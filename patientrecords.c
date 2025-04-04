#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAX_PATIENTS 1000
#define MAX_HISTORY_LENGTH 4096

typedef struct {
    char *name;
    char *id;
    char *medical_history;
} Patient;

typedef struct {
    Patient *patients[MAX_PATIENTS];
    int count;
} HospitalSystem;

// Initialize the hospital system
void init_hospital_system(HospitalSystem *system) {
    system->count = 0;
    for (int i = 0; i < MAX_PATIENTS; i++) {
        system->patients[i] = NULL;
    }
}

// Safely allocate memory with error checking
void *safe_malloc(size_t size) {
    void *ptr = malloc(size);
    if (ptr == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }
    return ptr;
}

// Safely duplicate string with error checking
char *safe_strdup(const char *str) {
    if (str == NULL) return NULL;
    
    char *new_str = strdup(str);
    if (new_str == NULL) {
        fprintf(stderr, "String duplication failed\n");
        exit(EXIT_FAILURE);
    }
    return new_str;
}

// Add a new patient to the system
bool add_patient(HospitalSystem *system, const char *name, const char *id, const char *medical_history) {
    if (system->count >= MAX_PATIENTS) {
        printf("SORRY! Can't add patient! Maximum capacity reached.\n");
        return false;
    }

    Patient *new_patient = (Patient *)safe_malloc(sizeof(Patient));
    new_patient->name = safe_strdup(name);
    new_patient->id = safe_strdup(id);
    new_patient->medical_history = safe_strdup(medical_history);

    // Find first available slot
    for (int i = 0; i < MAX_PATIENTS; i++) {
        if (system->patients[i] == NULL) {
            system->patients[i] = new_patient;
            system->count++;
            return true;
        }
    }

    // Should never reach here if MAX_PATIENTS is correct
    free(new_patient->name);
    free(new_patient->id);
    free(new_patient->medical_history);
    free(new_patient);
    return false;
}

// Find a patient by ID (returns index or -1 if not found)
int find_patient(HospitalSystem *system, const char *id) {
    for (int i = 0; i < MAX_PATIENTS; i++) {
        if (system->patients[i] != NULL && strcmp(system->patients[i]->id, id) == 0) {
            return i;
        }
    }
    return -1;
}

// Update a patient's medical history
bool update_medical_history(HospitalSystem *system, const char *id, const char *new_history) {
    int index = find_patient(system, id);
    if (index == -1) {
        printf("Patient not found.\n");
        return false;
    }

    // Check if new history is too long
    if (strlen(new_history) > MAX_HISTORY_LENGTH) {
        printf("Medical history too long. Maximum length is %d characters.\n", MAX_HISTORY_LENGTH);
        return false;
    }

    // Reallocate memory for the new history
    char *temp = realloc(system->patients[index]->medical_history, strlen(new_history) + 1);
    if (temp == NULL) {
        printf("Failed to update medical history due to memory constraints.\n");
        return false;
    }

    system->patients[index]->medical_history = temp;
    strcpy(system->patients[index]->medical_history, new_history);
    return true;
}

// Remove a patient from the system
bool remove_patient(HospitalSystem *system, const char *id) {
    int index = find_patient(system, id);
    if (index == -1) {
        printf("Patient not found.\n");
        return false;
    }

    free(system->patients[index]->name);
    free(system->patients[index]->id);
    free(system->patients[index]->medical_history);
    free(system->patients[index]);
    system->patients[index] = NULL;
    system->count--;

    return true;
}

// Display all patients
void display_patients(HospitalSystem *system) {
    printf("\nCurrent Patients (%d/%d):\n", system->count, MAX_PATIENTS);
    printf("--------------------------------------------------\n");
    for (int i = 0; i < MAX_PATIENTS; i++) {
        if (system->patients[i] != NULL) {
            printf("Name: %s\n", system->patients[i]->name);
            printf("ID: %s\n", system->patients[i]->id);
            printf("Medical History: %s\n", system->patients[i]->medical_history);
            printf("--------------------------------------------------\n");
        }
    }
}

// Clean up all memory when system is closed
void cleanup_system(HospitalSystem *system) {
    for (int i = 0; i < MAX_PATIENTS; i++) {
        if (system->patients[i] != NULL) {
            remove_patient(system, system->patients[i]->id);
        }
    }
}

int main() {
    HospitalSystem hospital;
    init_hospital_system(&hospital);

    int choice;
    char name[100], id[100], history[MAX_HISTORY_LENGTH];

    printf("Hospital Patient Records System\n");
    printf("-----------------------------\n");

    while (1) {
        printf("\nMenu:\n");
        printf("1. Add new patient\n");
        printf("2. Update medical history\n");
        printf("3. Remove patient\n");
        printf("4. Display all patients\n");
        printf("5. Exit\n");
        printf("Enter your choice: ");
        
        if (scanf("%d", &choice) != 1) {
            printf("Invalid input. Please enter a number.\n");
            while (getchar() != '\n'); // Clear input buffer
            continue;
        }

        switch (choice) {
            case 1:
                printf("Enter patient name: ");
                scanf(" %[^\n]", name);
                printf("Enter patient ID: ");
                scanf(" %[^\n]", id);
                printf("Enter medical history: ");
                scanf(" %[^\n]", history);
                
                if (!add_patient(&hospital, name, id, history)) {
                    printf("Failed to add patient.\n");
                } else {
                    printf("Patient added successfully.\n");
                }
                break;
                
            case 2:
                printf("Enter patient ID to update: ");
                scanf(" %[^\n]", id);
                printf("Enter new medical history: ");
                scanf(" %[^\n]", history);
                
                if (!update_medical_history(&hospital, id, history)) {
                    printf("Failed to update medical history.\n");
                } else {
                    printf("Medical history updated successfully.\n");
                }
                break;
                
            case 3:
                printf("Enter patient ID to remove: ");
                scanf(" %[^\n]", id);
                
                if (!remove_patient(&hospital, id)) {
                    printf("Failed to remove patient.\n");
                } else {
                    printf("Patient removed successfully.\n");
                }
                break;
                
            case 4:
                display_patients(&hospital);
                break;
                
            case 5:
                cleanup_system(&hospital);
                printf("Exiting system. Goodbye!\n");
                return 0;
                
            default:
                printf("Invalid choice. Please try again.\n");
        }
    }

    return 0;
}
