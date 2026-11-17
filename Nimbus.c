
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Patient
{
    int id;
    char name[50];
    char symptoms[100];
    int doctor;
    int emergency;
    int type;
    int status;
};

struct Doctor
{
    char name[50];
    char specialization[50];
};

struct Patient *p;
struct Doctor d[3] = {
    {"Dr. Sharma", "General Physician"},
    {"Dr. Verma", "Dentist"},
    {"Dr. Singh", "Cardiologist"}
};

int n = 0, nextId = 1;

void saveData()
{
    FILE *fp = fopen("patients.dat", "wb");

    if (fp == NULL)
    {
        printf("Error saving data!\n");
        return;
    }

    fwrite(&n, sizeof(int), 1, fp);
    fwrite(&nextId, sizeof(int), 1, fp);
    fwrite(p, sizeof(struct Patient), n, fp);

    fclose(fp);
}

void loadData()
{
    FILE *fp = fopen("patients.dat", "rb");

    p = malloc(1000 * sizeof(struct Patient));

    if (p == NULL)
    {
        printf("Memory error!\n");
        exit(1);
    }

    if (fp != NULL)
    {
        fread(&n, sizeof(int), 1, fp);
        fread(&nextId, sizeof(int), 1, fp);

        if (n < 0 || n > 1000)
        {
            n = 0;
            nextId = 1;
        }
        else
        {
            fread(p, sizeof(struct Patient), n, fp);
        }

        fclose(fp);
    }
}

void showDoctors()
{
    int i;

    printf("\n--- DOCTORS ---\n");

    for (i = 0; i < 3; i++)
    {
        printf("%d. %s - %s\n",
               i + 1, d[i].name, d[i].specialization);
    }
}

void bookPatient(int type)
{
    int i, doctor, emergency;
    char name[50], symptoms[100];

    if (n >= 1000)
    {
        printf("Patient list is full!\n");
        return;
    }

    showDoctors();

    printf("Choose doctor (1-3): ");
    scanf("%d", &doctor);

    if (doctor < 1 || doctor > 3)
    {
        printf("Invalid doctor!\n");
        return;
    }

    printf("Enter patient name: ");
    scanf(" %49[^\n]", name);

    printf("Enter symptoms: ");
    scanf(" %99[^\n]", symptoms);

    printf("Emergency? (1 for Yes, 0 for No): ");
    scanf("%d", &emergency);

    if (emergency != 1)
        emergency = 0;

    p[n].id = nextId++;
    strcpy(p[n].name, name);
    strcpy(p[n].symptoms, symptoms);
    p[n].doctor = doctor - 1;
    p[n].emergency = emergency;
    p[n].type = type;
    p[n].status = 0;

    printf("\nPatient added successfully!\n");
    printf("Patient ID: %d\n", p[n].id);
    printf("Doctor: %s\n", d[doctor - 1].name);

    n++;
    saveData();
}

void cancelAppointment()
{
    int id, i;

    printf("Enter patient ID to cancel: ");
    scanf("%d", &id);

    for (i = 0; i < n; i++)
    {
        if (p[i].id == id && p[i].status == 0)
        {
            p[i].status = 2;
            printf("Appointment cancelled!\n");
            saveData();
            return;
        }
    }

    printf("Waiting patient not found!\n");
}

void showPatients()
{
    int i;

    printf("\n--- PATIENT RECORDS ---\n");

    for (i = 0; i < n; i++)
    {
        printf("\nID: %d", p[i].id);
        printf("\nName: %s", p[i].name);
        printf("\nSymptoms: %s", p[i].symptoms);
        printf("\nDoctor: %s", d[p[i].doctor].name);

        if (p[i].emergency)
            printf("\nPriority: Emergency");
        else
            printf("\nPriority: Normal");

        if (p[i].status == 0)
            printf("\nStatus: Waiting");
        else if (p[i].status == 1)
            printf("\nStatus: Served");
        else
            printf("\nStatus: Cancelled");

        if (p[i].type == 1)
            printf("\nType: Appointment\n");
        else
            printf("\nType: Walk-in\n");
    }
}

void servePatient()
{
    int i, found = -1;

    /* Emergency patients get priority */

    for (i = 0; i < n; i++)
    {
        if (p[i].status == 0 && p[i].emergency == 1)
        {
            found = i;
            break;
        }
    }

    /* Otherwise serve the first waiting patient */

    if (found == -1)
    {
        for (i = 0; i < n; i++)
        {
            if (p[i].status == 0)
            {
                found = i;
                break;
            }
        }
    }

    if (found == -1)
    {
        printf("No patients are waiting!\n");
        return;
    }

    printf("\nNow serving patient: %s\n", p[found].name);
    printf("Doctor: %s\n", d[p[found].doctor].name);

    p[found].status = 1;
    saveData();
}

void reports()
{
    int i, j;
    int count[3] = {0, 0, 0};
    int waiting = 0, cancelled = 0;
    int waitTime = 0;

    for (i = 0; i < n; i++)
    {
        if (p[i].type == 1 && p[i].status != 2)
            count[p[i].doctor]++;

        if (p[i].status == 0)
        {
            waiting++;

            /* Simple waiting time estimate */

            j = 0;
            for (int k = 0; k < i; k++)
            {
                if (p[k].status == 0)
                    j++;
            }

            waitTime += (j + 1) * 15;
        }

        if (p[i].status == 2)
            cancelled++;
    }

    printf("\n--- CLINIC REPORT ---\n");

    for (i = 0; i < 3; i++)
    {
        printf("%s: %d appointments\n", d[i].name, count[i]);
    }

    printf("Patients waiting: %d\n", waiting);
    printf("Estimated average waiting time: %d minutes\n",
           waiting > 0 ? waitTime / waiting : 0);
    printf("Cancelled appointments: %d\n", cancelled);
}

int main()
{
    int choice;

    loadData();

    do
    {
        printf("\n===== CLINIC MANAGEMENT SYSTEM =====\n");
        printf("1. Book Appointment\n");
        printf("2. Add Walk-in Patient\n");
        printf("3. Cancel Appointment\n");
        printf("4. Serve Next Patient\n");
        printf("5. Show All Patients\n");
        printf("6. Generate Reports\n");
        printf("7. Show Doctors\n");
        printf("0. Exit\n");

        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice)
        {
            case 1:
                bookPatient(1);
                break;

            case 2:
                bookPatient(2);
                break;

            case 3:
                cancelAppointment();
                break;

            case 4:
                servePatient();
                break;

            case 5:
                showPatients();
                break;

            case 6:
                reports();
                break;

            case 7:
                showDoctors();
                break;

            case 0:
                saveData();
                printf("Data saved. Exiting program!\n");
                break;

            default:
                printf("Invalid choice! Try again.\n");
        }

    } while (choice != 0);

    free(p);

    return 0;
}
