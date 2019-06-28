#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>

int main(int argc, char **argv)
{
    // zmienne lokalne
    int server_socket, size;
    struct sockaddr_in server_info;
    unsigned char buffer[512], b;

    // utworzenie socketa
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
	server_info.sin_family = AF_INET;
	server_info.sin_addr.s_addr = inet_addr("127.0.0.1");
	server_info.sin_port = htons(5555);
    // połączenie z serwerem
    connect(server_socket, (struct sockaddr*)&server_info, sizeof(server_info));

    // główna pętla programu
    while (1)
    {
        // pobranie ścieżki do pliku zdalnego
        size = -1;
        printf("Podaj ścieżkę do zdalnego pliku: ");
        scanf("%s", buffer);
        // wysłanie ścieżki do serwera
        send(server_socket, buffer, strlen(buffer), 0);

        // pobranie odpowiedzi serwera (rozmiar pliku lub -1 jeżeli błędna ścieżka)
        if (recv(server_socket, &size, 4, 0) == 4)
        {
            // wypisz plik na konsolę
            if (size != -1)
            {
                while (size-- > 0)
                {
                    recv(server_socket, &b, 1, 0);
                    putchar(b);
                }
            }
            // wypisz błąd
            else
            {
                printf("Nieprawidłowa ścieżka!");
            }
            
            printf("\n");
        }
        else
        {
            break;
        }
        
    }
    
    // koniec programu klienta
    return 0;
}
