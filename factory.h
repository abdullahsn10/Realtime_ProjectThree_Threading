#ifndef __FACTORY_H_
#define __FACTORY_H_

#define QUEUE_SIZE 200

struct MEDICINE
{
    int medicine_id;
    char medicine_type; // 'L' for liquid and 'P" for pill
};

// liquids hdrs
struct BOTTLE
{
    int bottle_id;
    int medicine_id;
    int has_normal_liquid_level;
    int has_correct_color;
    int is_sealed;
    int has_correct_label;
    int has_correct_expiry_date;
};

struct BOTTLE_PACKAGE
{
    int package_id;
    struct BOTTLE bottle;
    char *folded_perspection;
};

struct PROD_LINE
{
    int line_id;
    int produced_medicine_id;
    int speed_of_line;
};

// pills hdrs
struct PLASTIC_CONTAINER
{
    int plastic_container_id;
    int medicine_id;
    int has_no_missed_pill;
    int has_correct_color;
    int has_correct_size;
    int has_correct_expiry_date;
};

struct PLASTIC_CONTAINER_ACKAGE
{
    int package_id;
    struct PLASTIC_CONTAINER plastic_container;
    char *folded_perspection;
};
struct SPEED_MESSAGE
{
    pthread_t thread_id;
    int reamining_medicine;
    int line_pid;
    int need_employee;
    int restore_employee;
    int is_sent_employee;
};
struct HISTORY
{
    int line_id_source;
    int line_id_dest;
    int is_back;
};


#endif