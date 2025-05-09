#include <pthread.h>

#include <chrono>
#include <cmath>
#include <iostream>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

const int ThreadGroups = 6;

struct ThreadArgs
{
    Mat inputImage;
    Mat resultImage;
    int numThreads;
    int threadID;
};

void *
applySobelFilterThread(void *args)
{
    ThreadArgs *threadArgs = (ThreadArgs *) args;

    // Проверка наличия данных в изображении
    if (threadArgs->inputImage.empty())
    {
        cerr << "Ошибка: изображение не загружено в поток "
             << threadArgs->threadID << "\n";
        pthread_exit(NULL);
    }

    int rowsPerThread = threadArgs->inputImage.rows / threadArgs->numThreads;
    int startRow      = threadArgs->threadID * rowsPerThread;
    int endRow        = (threadArgs->threadID == threadArgs->numThreads - 1)
                            ? threadArgs->inputImage.rows
                            : startRow + rowsPerThread;

    Mat gradX, gradY;
    Sobel(threadArgs->inputImage, gradX, CV_16S, 1, 0, 3);
    Sobel(threadArgs->inputImage, gradY, CV_16S, 0, 1, 3);

    for (int i = startRow; i < endRow; i++)
    {
        for (int j = 0; j < threadArgs->inputImage.cols; j++)
        {
            threadArgs->resultImage.at<uchar>(i, j) = sqrt(
                pow(gradX.at<short>(i, j), 2) + pow(gradY.at<short>(i, j), 2));
        }
    }
    pthread_exit(NULL);
}

int
main(int argc, char **argv)
{
    if (argc < 2)
    {
        cout << "Путь к изображению не указан!\n";
        exit(EXIT_FAILURE);
    }
    char *pathToInputImage = argv[1];
    Mat   inputImage       = imread(pathToInputImage, IMREAD_GRAYSCALE);

    if (inputImage.empty())
    {
        cout << "Ошибка открытия изображения " << pathToInputImage << "!\n";
        exit(EXIT_FAILURE);
    }

    Mat resultImage = Mat::zeros(inputImage.rows, inputImage.cols, CV_8UC1);

    int    numThreads[] = {1, 2, 4, 8, 16, 32};
    double timing;
    for (int i = 0; i < ThreadGroups; i++)
    {
        pthread_t  threads[numThreads[i]];
        ThreadArgs threadArgs[numThreads[i]];

        auto start = chrono::high_resolution_clock::now();

        for (int j = 0; j < numThreads[i]; j++)
        {
            threadArgs[j].inputImage  = inputImage;
            threadArgs[j].resultImage = resultImage;
            threadArgs[j].numThreads  = numThreads[i];
            threadArgs[j].threadID    = j;

            // Передаем правильный индекс аргументов потокам
            if (pthread_create(&threads[j], NULL, applySobelFilterThread,
                               &threadArgs[j]) != 0)
            {
                cerr << "Ошибка при создании потока " << j << "\n";
                exit(EXIT_FAILURE);
            }
        }

        for (int j = 0; j < numThreads[i]; j++)
        {
            pthread_join(threads[j], NULL);
        }

        auto end = chrono::high_resolution_clock::now();
        timing =
            chrono::duration_cast<chrono::microseconds>(end - start).count();

        cout << "Время выполнения с " << numThreads[i]
             << " потоками: " << timing << " микросекунд\n";
    }

    namedWindow("Input Image", WINDOW_NORMAL);
    namedWindow("Result Image", WINDOW_NORMAL);
    resizeWindow("Input Image", 800, 750);
    resizeWindow("Result Image", 800, 750);
    imshow("Input Image", inputImage);
    imshow("Result Image", resultImage);
    waitKey(0);
    destroyAllWindows();

    return 0;
}
