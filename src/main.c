# include <stdio.h>
# include <stdlib.h>
# include <stdbool.h>
# include <time.h>
# include <unistd.h>
# include <sys/types.h>
# include <sys/wait.h>
# include <sys/user.h>


void produtor(void) {
    if (fopen("buffer.txt.lock", "r") != NULL) {
        return;
    }
    
    FILE *lock = fopen("buffer.txt.lock", "w");
    FILE *buffer = fopen("buffer.txt", "a");
    int num_produtor = rand() % 100;

    printf("\033[1;32mprodutor:\033[0;0m %2d\n", num_produtor);
    fprintf(buffer ,"%d\n", num_produtor);

    fclose(buffer);
    
    remove("buffer.txt.lock");
}


bool consumidor(void) {
    if (fopen("buffer.txt.lock", "r") != NULL) {
        return 1;
    }

    FILE *lock = fopen("buffer.txt.lock", "w");
    FILE *buffer = fopen("buffer.txt", "r");
    FILE *temp    = fopen("temp.txt", "w");
    int num_consumidor;

    fscanf(buffer, "%d", &num_consumidor);
    printf("\033[1;31mconsumidor:\033[0;0m %2d\n", num_consumidor);

    while (fscanf( buffer, "%d", &num_consumidor) != EOF) {
        fprintf(temp ,"%d\n", num_consumidor);
    }

    fseek(temp, 0, SEEK_END);
    
    bool vazio = !ftell(temp);

    remove("buffer.txt");
    rename("temp.txt", "buffer.txt");
    
    remove("buffer.txt.lock");

    if (vazio)
        return false;
    else
        return true;
}


int main(void) {
    srand(time(NULL));
    FILE *buffer = fopen("buffer.txt", "w");

    for (int i = 0; i < 10; ++i) {   
        fprintf(buffer ,"%d\n", rand() % 100);
    }
    
    fclose(buffer);

    bool status = 1;
    int pid = fork();
    while (status != 0) {
        if(pid == 0) {
            produtor();
            sleep(rand() % 4);
        } 
        
        else {
            status = consumidor();
            sleep(rand() % 4);
        }
    }

    kill(pid, SIGKILL);
    
    remove("buffer.txt");
    
    return 0;
}
