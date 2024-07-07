
#include "header.h"
#include "functions.h"

// Window dimensions
#define WIDTH 800
#define HEIGHT 600

// Maximum number of production lines
#define MAX_PRODUCTION_LINES 10
int count_current_production_lines;

// Structure to hold production line information
typedef struct
{
    char type; // 'L' for liquid, 'P' for pill
    int numInspectingWorkers;
    int numPackagingWorkers;
    int line_speed;
    int produced;
    int inspected;
    int packaged;
    int passed;
    int denied;
    int total;

} ProductionLine;

ProductionLine productionLines[MAX_PRODUCTION_LINES];
int productionLineCount = 0;

// initialize global variables for public fifo
int n, done, dummyfifo, publicfifo, privatefifo;

// Function prototypes
void init();
void display();
void drawProductionLine(float x, float y, float width, float height, const char *label, float r, float g, float b);
void drawText(float x, float y, const char *text);
void drawArrow(float x1, float y1, float x2, float y2);
void drawFactoryElements();
void updateCounts();
void addProductionLine(int id, char type, int numInspectingWorkers, int numPackagingWorkers, int line_speed, int produced, int inspected, int packaged, int passed, int denied, int total);
void drawProductionLines();
int check_threshold_of_defective(char type);
int check_threshold_of_produced(char type);
// Production statistics (for simplicity, hard-coded here)
int produced_bottles_liquid[] = {20};
int inspected_bottles_liquid[] = {18};
int passed_bottles_liquid[] = {15};
int denied_bottles_liquid[] = {3};
int packaged_bottles_liquid[] = {5};
int total_bottles_liquid[] = {5};

int produced_bottles_pill[] = {30};
int inspected_bottles_pill[] = {25};
int passed_bottles_pill[] = {20};
int denied_bottles_pill[] = {5};
int packaged_bottles_pill[] = {10};
int total_bottles_pill[] = {10};

int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(WIDTH, HEIGHT);
    glutCreateWindow("Pharmaceutical Factory Simulation");

    init();
    /*
        // Add some production lines for demonstration
        addProductionLine('L', 3, 2); // Liquid line with 3 inspecting and 2 packaging workers
        addProductionLine('P', 4, 3); // Pill line with 4 inspecting and 3 packaging workers
        addProductionLine('L', 5, 3); // Another Liquid line with 5 inspecting and 3 packaging workers
        addProductionLine('P', 6, 4); // Another Pill line with 6 inspecting and 4 packaging workers
    */
    glutDisplayFunc(display);
    glutIdleFunc(updateCounts);
    glutMainLoop();
    return 0;
}

void init()

{
    read_settings_from_a_file("pill_settings.txt");
    read_settings_from_a_file("liquid_settings.txt");
    read_settings_from_a_file("opengl_settings.txt");

    glClearColor(1.0, 1.0, 1.0, 1.0); // Set background color to white
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0.0, WIDTH, 0.0, HEIGHT);

    // make opengl create the main process
    int pid;
    // create the main function using opengl
    if ((pid = fork()) == -1)
    {
        perror("error in fork\n");
        exit(-1);
    }
    else if (pid == 0)
    {
        execlp("./main", "main", NULL);
        perror("Error in exec\n");
        exit(-1);
    }
    else if (pid > 0)
    {

        FILE *pf;

        printf("first\n");

        if (access(PUBLIC, F_OK) != -1)
        {
            // File exists, remove it
            if (unlink(PUBLIC) == -1)
            {
                perror("Error removing existing FIFO");
                exit(EXIT_FAILURE);
            }
        }
        // create the PUBLIC fifo in opengl
        if ((mknod(PUBLIC, __S_IFIFO | 0777, 0)) == -1)
        {
            perror("Error");
            exit(-1);
        }
        printf("Second\n");
        //  create public fifo as read only in (opengl)
        if ((publicfifo = open(PUBLIC, O_RDONLY | O_NONBLOCK)) == -1)
        {

            perror(PUBLIC);
            exit(1);
        }
        printf("End fifo\n");
    }

    for (int i = 0; i < NUMBER_OF_PRODUCTION_LINES; i++)
    {
        productionLines[i].numInspectingWorkers = 0;
        productionLines[i].numPackagingWorkers = 0;
        productionLines[i].type = '\0';
        productionLines[i].line_speed = 0;
        productionLines[i].produced = 0;
        productionLines[i].inspected = 0;
        productionLines[i].packaged = 0;
    }
}

void display()
{
    glClear(GL_COLOR_BUFFER_BIT);

    // begin display of opengl
    struct message msg;

    if (read(publicfifo, (char *)&msg, sizeof(msg)) > 0)
    {
        // printf("Read:%s\n", msg.cmd_line);

        // read message and divide each part
        char message[100];
        strcpy(message, msg.cmd_line);
        // printf("%s\n", msg.cmd_line);
        char firstLetter = msg.cmd_line[0];

        char *allString;
        allString = strtok(msg.cmd_line, ",");
        char letter = allString[0];

        if (firstLetter == 'A')
        {

            allString = strtok(NULL, ",");
            int liquid_production_line_id = atoi(allString);
            liquid_production_line_id -= 1;

            allString = strtok(NULL, ",");
            int line_speed = atoi(allString);

            // Add some production lines for demonstration
            addProductionLine(liquid_production_line_id, 'L', productionLines[liquid_production_line_id].numInspectingWorkers,
                              productionLines[liquid_production_line_id].numPackagingWorkers,
                              productionLines[liquid_production_line_id].line_speed, productionLines[liquid_production_line_id].produced,
                              productionLines[liquid_production_line_id].inspected, productionLines[liquid_production_line_id].packaged,
                              productionLines[liquid_production_line_id].passed, productionLines[liquid_production_line_id].denied, productionLines[liquid_production_line_id].total);
        }
        else if (firstLetter == 'B')
        {

            allString = strtok(NULL, ",");
            int liquid_production_line_id = atoi(allString);
            liquid_production_line_id -= 1;
            allString = strtok(NULL, ",");
            int line_speed = atoi(allString);

            printf("line_id = %d(B)\n", liquid_production_line_id);

            // Add some production lines for demonstration

            // Add some production lines for demonstration
            addProductionLine(liquid_production_line_id, 'P', productionLines[liquid_production_line_id].numInspectingWorkers,
                              productionLines[liquid_production_line_id].numPackagingWorkers,
                              productionLines[liquid_production_line_id].line_speed, productionLines[liquid_production_line_id].produced,
                              productionLines[liquid_production_line_id].inspected, productionLines[liquid_production_line_id].packaged,
                              productionLines[liquid_production_line_id].passed, productionLines[liquid_production_line_id].denied, productionLines[liquid_production_line_id].total);
        }
        else if (firstLetter == 'C')
        {
            // create new inspecting employee for liquid line
            allString = strtok(NULL, ",");
            int line_id = atoi(allString);
            line_id -= 1;
            allString = strtok(NULL, ",");
            int line_speed = atoi(allString);

            int count = 0;
            int i;
            for (i = 0; i < MAX_PRODUCTION_LINES; i++)
            {

                if (i == line_id && productionLines[i].type == 'L')
                {
                    productionLines[i].line_speed = line_speed;
                    productionLines[i].numInspectingWorkers += 1;
                    break;
                }
            }
        }
        else if (firstLetter == 'D')
        {
            // create new packeging employee for liquid line
            allString = strtok(NULL, ",");
            int line_id = atoi(allString);
            line_id -= 1;
            allString = strtok(NULL, ",");
            int line_speed = atoi(allString);

            int count = 0;

            int i;
            for (i = 0; i < MAX_PRODUCTION_LINES; i++)
            {

                if (i == line_id && productionLines[i].type == 'L')
                {
                    productionLines[i].line_speed = line_speed;
                    productionLines[i].numPackagingWorkers += 1;
                    break;
                }
            }
        }
        else if (firstLetter == 'E')
        {
            // create new inspecting employee for pill line
            allString = strtok(NULL, ",");
            int line_id = atoi(allString);
            line_id -= 1;
            allString = strtok(NULL, ",");
            int line_speed = atoi(allString);

            int count = 0;

            int i;
            for (i = 0; i < MAX_PRODUCTION_LINES; i++)
            {

                if (i == line_id && productionLines[i].type == 'P')
                {
                    productionLines[i].line_speed = line_speed;
                    productionLines[i].numInspectingWorkers += 1;
                    break;
                }
            }
        }
        else if (firstLetter == 'F')
        {
            // create new packaging employee for pill line
            allString = strtok(NULL, ",");
            int line_id = atoi(allString);
            line_id -= 1;
            allString = strtok(NULL, ",");
            int line_speed = atoi(allString);

            int count = 0;

            int i;
            for (i = 0; i < MAX_PRODUCTION_LINES; i++)
            {

                if (i == line_id && productionLines[i].type == 'P')
                {
                    productionLines[i].line_speed = line_speed;
                    productionLines[i].numPackagingWorkers += 1;
                    break;
                }
            }
        }
        else if (firstLetter == 'G')
        {
            // number of produced bottles in liquid lines
            allString = strtok(NULL, ",");
            int line_id = atoi(allString);
            line_id -= 1;
            allString = strtok(NULL, ",");
            int number_of_produced = atoi(allString);
            productionLines[line_id].produced = number_of_produced;
        }
        else if (firstLetter == 'H')
        {
            // number of inspected bottles in liquid line
            allString = strtok(NULL, ",");
            int line_id = atoi(allString);
            line_id -= 1;
            allString = strtok(NULL, ",");
            int number_of_inspected = atoi(allString);
            productionLines[line_id].inspected = number_of_inspected;
        }
        else if (firstLetter == 'I')
        {
            // number of passed bottles in liquid line
            allString = strtok(NULL, ",");
            int line_id = atoi(allString);
            line_id -= 1;
            allString = strtok(NULL, ",");
            int number_of_passed = atoi(allString);
            productionLines[line_id].passed = number_of_passed;
        }
        else if (firstLetter == 'J')
        {
            // number of failed bottles in liquid line
            allString = strtok(NULL, ",");
            int line_id = atoi(allString);
            line_id -= 1;
            allString = strtok(NULL, ",");
            int number_of_failed = atoi(allString);
            productionLines[line_id].denied = number_of_failed;
        }
        else if (firstLetter == 'K')
        {
            // number of packaged bottles in liquid line
            allString = strtok(NULL, ",");
            int line_id = atoi(allString);
            line_id -= 1;
            allString = strtok(NULL, ",");
            int number_of_packages = atoi(allString);
            productionLines[line_id].packaged = number_of_packages;
        }
        else if (firstLetter == 'L')
        {
            // number of total bottles in liquid line
            allString = strtok(NULL, ",");
            int line_id = atoi(allString);
            line_id -= 1;
            allString = strtok(NULL, ",");
            int number_of_total = atoi(allString);
            productionLines[line_id].total = number_of_total;
        }
        else if (firstLetter == 'M')
        {

            // number of produced bottles in pill line
            allString = strtok(NULL, ",");
            int line_id = atoi(allString);
            line_id -= 1;
            allString = strtok(NULL, ",");
            int number_of_produced_bottles = atoi(allString);
            printf("----M--------------------------------\n");
            printf("line_id = %d\n", line_id);
            productionLines[line_id].produced = number_of_produced_bottles;
        }
        else if (firstLetter == 'N')
        {
            // number of inspected bottles in pill line
            allString = strtok(NULL, ",");
            int line_id = atoi(allString);
            line_id -= 1;
            allString = strtok(NULL, ",");
            int number_of_inspected_bottles = atoi(allString);
            productionLines[line_id].inspected = number_of_inspected_bottles;
        }
        else if (firstLetter == 'O')
        {
            // number of passed bottles in pill line
            allString = strtok(NULL, ",");
            int line_id = atoi(allString);
            line_id -= 1;
            allString = strtok(NULL, ",");
            int number_of_passed = atoi(allString);
            productionLines[line_id].passed = number_of_passed;
        }
        else if (firstLetter == 'P')
        {
            // number of failed bottles in pill line
            allString = strtok(NULL, ",");
            int line_id = atoi(allString);
            line_id -= 1;
            allString = strtok(NULL, ",");
            int number_of_failed = atoi(allString);
            productionLines[line_id].denied = number_of_failed;
        }
        else if (firstLetter == 'Q')
        {
            // number of packges in pill line
            allString = strtok(NULL, ",");
            int line_id = atoi(allString);
            line_id -= 1;
            allString = strtok(NULL, ",");
            int number_of_packages = atoi(allString);
            productionLines[line_id].packaged = number_of_packages;
        }
        else if (firstLetter == 'R')
        {
            // number of total in pill line
            allString = strtok(NULL, ",");
            int line_id = atoi(allString);
            line_id -= 1;
            allString = strtok(NULL, ",");
            int number_of_total = atoi(allString);
            productionLines[line_id].total = number_of_total;
        }
        else if (firstLetter == 'S')
        {
            // reduce number of employees in liquid line:
            allString = strtok(NULL, ",");
            int line_id = atoi(allString);
            line_id -= 1;
            allString = strtok(NULL, ",");
            int line_speed = atoi(allString);

            int count = 0;
            int i;
            for (i = 0; i < MAX_PRODUCTION_LINES; i++)
            {

                if (i == line_id && productionLines[i].type == 'L')
                {
                    productionLines[i].line_speed = line_speed;
                    productionLines[i].numInspectingWorkers -= 1;
                    break;
                }
            }
        }
        else if (firstLetter == 'T')
        {
            // reduce number of inspecting employees in pill line
            allString = strtok(NULL, ",");
            int line_id = atoi(allString);
            line_id -= 1;
            allString = strtok(NULL, ",");
            int line_speed = atoi(allString);

            int count = 0;

            int i;
            for (i = 0; i < MAX_PRODUCTION_LINES; i++)
            {

                if (i == line_id && productionLines[i].type == 'P')
                {
                    productionLines[i].line_speed = line_speed;
                    productionLines[i].numInspectingWorkers -= 1;
                    break;
                }
            }
        }
    }


    // Check if the number of failed bottles in any line is greater than the threshold
    // if so, kill openGL process
    if (check_threshold_of_defective('L') || check_threshold_of_defective('P'))
    {
        printf("[THRESHOLD]: Threshold of defective bottles/plastic exceeded. Terminating OpenGL process.\n");
        kill(getpid(), SIGKILL);
    }

    // Check if the number of produced bottles in any line is greater than the threshold
    // if so, kill openGL process
    if (check_threshold_of_produced('L') || check_threshold_of_produced('P'))
    {
        printf("[THRESHOLD]: Threshold of produced bottles/plastic exceeded. Terminating OpenGL process.\n");
        kill(getpid(), SIGKILL);
    }


    // Draw factory elements
    drawFactoryElements();

    // Draw production lines dynamically
    drawProductionLines();

    glutSwapBuffers();
}

void drawProductionLine(float x, float y, float width, float height, const char *label, float r, float g, float b)
{
    glColor3f(r, g, b); // Color based on production line type
    glBegin(GL_QUADS);
    glVertex2f(x, y);
    glVertex2f(x + width, y);
    glVertex2f(x + width, y + height);
    glVertex2f(x, y + height);
    glEnd();

    // Draw label
    drawText(x + width / 2 - 20, y + height + 10, label);
}

void drawText(float x, float y, const char *text)
{
    glColor3f(0.0, 0.0, 0.0); // Black color

    // Set the raster position
    glRasterPos2f(x, y);

    // Draw each character multiple times with slight offsets for bold effect
    for (const char *c = text; *c != '\0'; c++)
    {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
    }

    // Additional loops to make the text bold
    for (int i = 0; i < 2; i++)
    {
        glRasterPos2f(x + i * 0.5, y);
        for (const char *c = text; *c != '\0'; c++)
        {
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
        }
        glRasterPos2f(x, y + i * 0.5);
        for (const char *c = text; *c != '\0'; c++)
        {
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
        }
    }
}

void drawArrow(float x1, float y1, float x2, float y2)
{
    glColor3f(0.0, 0.0, 0.0); // Black color
    glBegin(GL_LINES);
    glVertex2f(x1, y1);
    glVertex2f(x2, y2);
    glEnd();

    // Draw arrowhead
    float angle = atan2(y2 - y1, x2 - x1);
    float arrowLength = 10.0;
    float arrowAngle = M_PI / 6;

    glBegin(GL_LINES);
    glVertex2f(x2, y2);
    glVertex2f(x2 - arrowLength * cos(angle - arrowAngle), y2 - arrowLength * sin(angle - arrowAngle));
    glVertex2f(x2, y2);
    glVertex2f(x2 - arrowLength * cos(angle + arrowAngle), y2 - arrowLength * sin(angle + arrowAngle));
    glEnd();
}

void drawFactoryElements()
{
    // Draw some factory elements to make it look more like a pharmaceutical factory
    glColor3f(0.7, 0.7, 0.7); // Gray color for machines
    glBegin(GL_QUADS);
    glVertex2f(50, 500);
    glVertex2f(100, 500);
    glVertex2f(100, 550);
    glVertex2f(50, 550);
    glVertex2f(700, 500);
    glVertex2f(750, 500);
    glVertex2f(750, 550);
    glVertex2f(700, 550);
    glEnd();
}

void updateCounts()
{
    // Update counts here if needed
    // For now, just trigger a redisplay
    glutPostRedisplay();
}

void addProductionLine(int id, char type, int numInspectingWorkers, int numPackagingWorkers, int lineSpeed, int produced, int inspected, int packaged, int passed, int denied, int total)
{
    if (productionLineCount < MAX_PRODUCTION_LINES)
    {
        ProductionLine line;
        line.type = type;
        line.numInspectingWorkers = numInspectingWorkers;
        line.numPackagingWorkers = numPackagingWorkers;
        line.line_speed = lineSpeed;
        line.produced = produced;
        line.inspected = inspected;
        line.packaged = packaged;
        line.passed = passed;
        line.denied = denied;
        line.total = total;
        productionLines[id] = line;
        productionLineCount++;
    }
}

void drawProductionLines()
{
    for (int i = 0; i < productionLineCount; i++)
    {
        float y = 400 - i * 100; // Adjust vertical position for each line
        if (productionLines[i].type == 'L')
        {
            drawProductionLine(100, y, 600, 50, "Liquid Line", 0.53, 0.81, 0.92); // Skyblue color

            // Draw arrows and text inside the liquid line
            char producedText[50];
            char inspectedText[50];
            char packagedText[50];
            char totalText[50];
            sprintf(producedText, "Produced: %d", productionLines[i].produced);
            sprintf(inspectedText, "Inspected: %d (Passed: %d, Denied: %d)", productionLines[i].inspected, productionLines[i].passed, productionLines[i].denied);
            sprintf(packagedText, "Packaged: %d", productionLines[i].packaged);
            sprintf(totalText, "Total: %d", productionLines[i].total);
            drawText(110, y + 25, producedText);
            drawArrow(160, y + 25, 210, y + 25);
            drawText(220, y + 25, inspectedText);
            drawArrow(410, y + 25, 460, y + 25);
            drawText(470, y + 25, packagedText);
            drawArrow(520, y + 25, 570, y + 25);
            drawText(580, y + 25, totalText);

            // Draw worker counts above the line
            char workerText[50];
            snprintf(workerText, sizeof(workerText), "Inspectors: %d", productionLines[i].numInspectingWorkers);
            drawText(200, y + 60, workerText);
            snprintf(workerText, sizeof(workerText), "Packagers: %d", productionLines[i].numPackagingWorkers);
            drawText(450, y + 60, workerText);
        }
        else if (productionLines[i].type == 'P')
        {

            drawProductionLine(100, y, 600, 50, "Pill Line", 0.0, 1.0, 0.0); // Green color

            char producedText[50];
            char inspectedText[50];
            char packagedText[50];
            char totalText[50];

            sprintf(producedText, "Produced: %d", productionLines[i].produced);
            sprintf(inspectedText, "Inspected: %d (Passed: %d, Denied: %d)", productionLines[i].inspected, productionLines[i].passed, productionLines[i].denied);
            sprintf(packagedText, "Packaged: %d", productionLines[i].packaged);
            sprintf(totalText, "Total: %d", productionLines[i].total);

            // Draw arrows and text inside the pill line
            drawText(110, y + 25, producedText);
            drawArrow(160, y + 25, 210, y + 25);
            drawText(220, y + 25, inspectedText);
            drawArrow(410, y + 25, 460, y + 25);
            drawText(470, y + 25, packagedText);
            drawArrow(520, y + 25, 570, y + 25);
            drawText(580, y + 25, totalText);

            // Draw worker counts above the line
            char workerText[50];
            snprintf(workerText, sizeof(workerText), "Inspectors: %d", productionLines[i].numInspectingWorkers);
            drawText(200, y + 60, workerText);
            snprintf(workerText, sizeof(workerText), "Packagers: %d", productionLines[i].numPackagingWorkers);
            drawText(450, y + 60, workerText);
        }
    }



}

// function to sum all the failed bottles in all lines and check if it is greater than the threshold
// if it is greater than the threshold, then the function will return 1, otherwise it will return 0
// check the type, create the function 
int check_threshold_of_defective(char type)
{
    int i;
    int sum = 0;
    if (type == 'L')
    {
        for (i = 0; i < MAX_PRODUCTION_LINES; i++)
        {
            if (productionLines[i].type == 'L')
            {
                sum += productionLines[i].denied;
            }
        }
        if (sum > THRESHOLD_OF_DEFECTIVE_LIQUIDS)
        {
            return 1;
        }
        else
        {
            return 0;
        }
    }
    else if (type == 'P')
    {
        for (i = 0; i < MAX_PRODUCTION_LINES; i++)
        {
            if (productionLines[i].type == 'P')
            {
                sum += productionLines[i].denied;
            }
        }
        if (sum > THRESHOLD_OF_DEFECTIVE_PILLS)
        {
            return 1;
        }
        else
        {
            return 0;
        }
    }
}


// function that accepts the type as above, and check the total produced in all lines of that type
// if the total produced is greater than the threshold, then the function will return 1, otherwise it will return 0
int check_threshold_of_produced(char type)
{
    int i;
    int sum = 0;
    if (type == 'L')
    {
        for (i = 0; i < MAX_PRODUCTION_LINES; i++)
        {
            if (productionLines[i].type == 'L')
            {
                sum += productionLines[i].produced;
            }
        }
        if (sum > THRESHOLD_OF_NEEDED_LIQUID_MEDICINES)
        {
            return 1;
        }
        else
        {
            return 0;
        }
    }
    else if (type == 'P')
    {
        for (i = 0; i < MAX_PRODUCTION_LINES; i++)
        {
            if (productionLines[i].type == 'P')
            {
                sum += productionLines[i].produced;
            }
        }
        if (sum > THRESHOLD_OF_NEEDED_PILL_MEDICINES)
        {
            return 1;
        }
        else
        {
            return 0;
        }
    }
}
