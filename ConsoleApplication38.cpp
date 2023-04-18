#include <string>
#include <fstream>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <filesystem>
#include <cstdlib>
#include <chrono>

#define PART_SIZE 500000
#define RAND_ARR_SIZE 20000000 

using namespace std;

void print(int* array, int n)
{
    for (int i = 0; i < n; i++)
        cout << array[i] << " ";
    cout << endl;
}

void change(int* a, int* b)
{
    int t = *a;
    *a = *b;
    *b = t;
}

int section(int*& array, int low, int high)
{
    int pivot = array[high];
    int i = (low - 1);

    for (int j = low; j <= high - 1; j++)
    {
        if (array[j] <= pivot)
        {
            i++;
            change(&array[i], &array[j]);
        }
    }
    change(&array[i + 1], &array[high]);
    return (i + 1);
}

void sort(int*& array, int low, int high)
{
    if (low < high)
    {
        int pi = section(array, low, high);
        sort(array, low, pi - 1);
        sort(array, pi + 1, high);
    }
}

void get(const int* array1, const int* array2, int size1, int size2)
{

    fstream temp;
    const int* first;
    const int* second;

    temp.open("temp_1_file.txt", fstream::out | ofstream::trunc);

    if (array1[0] < array2[0]) {
        first = array1;
        second = array2;
    }
    else {
        first = array2;
        second = array1;
        change(&size1, &size2);
    }

    if (temp.is_open())
    {
        int i = 0;
        int j = 0;

        while (i < size1 && j < size2) {
            if (first[i] < second[j])
                temp << first[i++] << ' ';
            else if (first[i] == second[j])
            {
                temp << first[i++] << ' ';
                temp << second[j++] << ' ';
            }
            else
                temp << second[j++] << ' ';
        }

        while (i < size1)
            temp << first[i++] << ' ';

        while (j < size2)
            temp << second[j++] << ' ';

        temp.close();
    }
}

void merge_files()
{
    fstream res;
    fstream temp1;
    fstream temp2;

    temp1.open("temp_1_file.txt", fstream::in);
    res.open("res_file.txt", fstream::in);
    temp2.open("temp_2_file.txt", fstream::out | ofstream::trunc); 

    if (!temp1.is_open() || !temp2.is_open() || !res.is_open())
        return;

    int temp1_value;
    int res_value;

    temp1 >> temp1_value;
    res >> res_value;
    while (!temp1.eof() && !res.eof()) {
        if (temp1_value <= res_value) {
            temp2 << temp1_value << ' ';
            temp1 >> temp1_value;
        }
        else {
            temp2 << res_value << ' ';
            res >> res_value;
        }
    }

    while (!res.eof()) {
        temp2 << res_value << ' ';
        res >> res_value;
    }

    while (!temp1.eof()) {
        temp2 << temp1_value << ' ';
        temp1 >> temp1_value;
    }

    temp1.close();
    temp2.close();
    res.close();

    res.open("res_file.txt", std::ofstream::out | std::ofstream::trunc);
    if (res.is_open())
        res.close();

    if (!filesystem::copy_file("temp_2_file.txt", "res_file.txt",
        filesystem::copy_options::overwrite_existing))
        return;
}

int read_part_arr(fstream& fs, int*& array)
{
    array = new int[PART_SIZE];
    int* tmp_arr;
    int i;
    for (i = 0; i < PART_SIZE && !fs.eof(); i++)
        fs >> array[i];

    if (i == 1) {
        delete[] array;
        return 0;
    }

    if (i != PART_SIZE) {
        tmp_arr = new int[i];
        for (size_t j = 0; j < i; j++)
            tmp_arr[j] = array[j];

        delete[] array;
        array = tmp_arr;
        return i - 1;
    }

    return PART_SIZE;
}

void sort_func(const string& filename) {

    fstream fs;
    fs.open(filename, fstream::in);

    if (fs.is_open())
    {
        while (!fs.eof())
        {
            int* part_1;
            int* part_2;

            int size_1 = read_part_arr(fs, part_1);
            int size_2 = read_part_arr(fs, part_2);
            if (size_1 == 0 || size_2 == 0)
                return;
            cout << " size_1 = " << size_1 << " size_2 = " << size_2 << endl;
            sort(part_1, 0, size_1 - 1);
            sort(part_2, 0, size_2 - 1);
            get(part_1, part_2, size_1, size_2);
            merge_files();
        }
        fs.close();
    }
}

void write_rand_arr(const string& filename)
{
    fstream fs;

    srand(time(nullptr));
    int lef_border = -100;
    int range_len = 50000; 

    fs.open(filename, fstream::out | ofstream::trunc);
    if (fs.is_open())
    {
        for (int i = 0; i < RAND_ARR_SIZE; i++)
            fs << (lef_border + rand() % range_len) << ' ';

        fs.close();
    }
}

void read_arr(const string& filename, int*& array, const int& n)
{
    fstream fs;

    fs.open(filename, fstream::in);
    if (fs.is_open()) 
    {
        array = new int[n];
        for (int i = 0; i < n; i++)
            fs >> array[i];

        fs.close();
    }
}

int main(int argc, char const* argv[])
{

    string filename = "array_data.txt";

    write_rand_arr(filename);
    cout << "Genetration is done!" << endl;

    fstream res;
    res.open("res_file.txt", fstream::out | ofstream::trunc);
    res.close();

    auto start = chrono::high_resolution_clock::now();
    sort_func(filename);
    auto finish = chrono::high_resolution_clock::now();
    chrono::duration<double> elapsed = finish - start;
    cout << "Elapsed time: " << elapsed.count() << " sec" << endl;

    return 0;
}