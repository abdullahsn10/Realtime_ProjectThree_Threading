#ifndef __FUNCTIONS__
#define __FUNCTIONS__

#include "header.h"
#include "constants.h"
#include "random_lib.h"

// reading from a file and updating values directly from constants.h
void read_settings_from_a_file(char *filename)
{
    char line[255];
    char label[50];

    FILE *file;
    file = fopen(filename, "r");
    if (file == NULL)
    {
        perror("Failed to open the file\n");
        exit(-2); // return failure status code
    }

    // separator of the file
    char separator[] = "=";

    // read the file line by line until reaching null
    while (fgets(line, sizeof(line), file) != NULL)
    {
        // split line
        char *str = strtok(line, separator);
        strncpy(label, str, sizeof(label));
        str = strtok(NULL, separator);

        // update arguments according to the label
        if (strcmp(label, "NUMBER_OF_DIFFERENT_MEDICINES") == 0)
        {
            NUMBER_OF_DIFFERENT_MEDICINES = atoi(str);
        }
        else if (strcmp(label, "NUMBER_OF_PRODUCTION_LINES") == 0)
        {
            NUMBER_OF_PRODUCTION_LINES = atoi(str);
        }
        else if (strcmp(label, "THRESHOLD_OF_NEEDED_LIQUID_MEDICINES") == 0)
        {
            THRESHOLD_OF_NEEDED_LIQUID_MEDICINES = atoi(str);
        }
        else if (strcmp(label, "THRESHOLD_OF_NEEDED_PILL_MEDICINES") == 0)
        {
            THRESHOLD_OF_NEEDED_PILL_MEDICINES = atoi(str);
        }
        else if (strcmp(label, "THRESHOLD_OF_DEFECTIVE_LIQUIDS") == 0)
        {
            THRESHOLD_OF_DEFECTIVE_LIQUIDS = atoi(str);
        }
        else if (strcmp(label, "THRESHOLD_OF_DEFECTIVE_PILLS") == 0)
        {
            THRESHOLD_OF_DEFECTIVE_PILLS = atoi(str);
        }
        else if (strcmp(label, "SIMULATION_TIME") == 0)
        {
            SIMULATION_TIME = atoi(str);
        }
        else if (strcmp(label, "MIN_LIQUID_LEVEl") == 0)
        {
            MIN_LIQUID_LEVEl = atoi(str);
        }
        else if (strcmp(label, "MAX_LIQUID_LEVEL") == 0)
        {
            MAX_LIQUID_LEVEL = atoi(str);
        }
        else if (strcmp(label, "NUMBER_OF_PILLS_IN_PLASTIC_CONTAINER") == 0)
        {
            NUMBER_OF_PILLS_IN_PLASTIC_CONTAINER = atoi(str);
        }
        else if (strcmp(label, "MIN_THROUGHPUT_OF_PRODUCTION_LINE") == 0)
        {
            MIN_THROUGHPUT_OF_PRODUCTION_LINE = atoi(str);
        }
        else if (strcmp(label, "MAX_THROUGHPUT_OF_PRODUCTION_LINE") == 0)
        {
            MAX_THROUGHPUT_OF_PRODUCTION_LINE = atoi(str);
        }
        else if (strcmp(label, "MIN_NUMBER_OF_INSPECTING_WORKERS") == 0)
        {
            MIN_NUMBER_OF_INSPECTING_WORKERS = atoi(str);
        }
        else if (strcmp(label, "MIN_NUMBER_OF_PACKAGING_WORKERS") == 0)
        {
            MIN_NUMBER_OF_PACKAGING_WORKERS = atoi(str);
        }
        else if (strcmp(label, "MAX_NUMBER_OF_INSPECTING_WORKERS") == 0)
        {
            MAX_NUMBER_OF_INSPECTING_WORKERS = atoi(str);
        }
        else if (strcmp(label, "MAX_NUMBER_OF_PACKAGING_WORKERS") == 0)
        {
            MAX_NUMBER_OF_PACKAGING_WORKERS = atoi(str);
        }
        
    }

    fclose(file);
}

void initialize_workers_numbers(){
    NUMBER_OF_PACKAGING_WORKERS=get_random_number_in_range(MIN_NUMBER_OF_PACKAGING_WORKERS, MAX_NUMBER_OF_PACKAGING_WORKERS);
    NUMBER_OF_INSPECTING_WORKERS=get_random_number_in_range(MIN_NUMBER_OF_INSPECTING_WORKERS, MAX_NUMBER_OF_INSPECTING_WORKERS);
}

#endif
