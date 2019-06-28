#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>

// funkcja obsługująca połączenie z klientem w nowym wątku
void *thread_function(void *parameter)
{
	// zmienne lokalne
	int client_socket, size, file_size;
	unsigned char buffer[512], b;
	FILE *file;

	// pobranie socketa
	client_socket = (*(int*)parameter);
	
	// dopóki można czytać z socketa
	while ((size = recv(client_socket, buffer, sizeof(buffer), 0) > 0))
	{
		// spróbuj otworzyć plik wskazany przez klienta
		file = fopen(buffer, "rb");

		// jeżeli się udało
		if (file != 0)
		{
			// pobierz rozmiar pliku
			fseek(file, 0L, SEEK_END);
			file_size = ftell(file);
			rewind(file);
			// wyślij rozmiar pliku
			write(client_socket, &file_size, 4);

			// dopóki nie odczytano całego pliku
			while (file_size-- > 0)
			{
				// czytaj i wyślij kolejny bajt
				b = getc(file);
				write(client_socket, &b, 1);
			}

			fclose(file);
		}
		// nieprawidłowa ścieżka
		else
		{
			// wyślij -1
			file_size = -1;
			write(client_socket, &file_size, 4);
		}
	}
}

int main(int argc, char **argv)
{
	// zmienne lokalne
	int server_socket, client_socket, c;
	struct sockaddr_in server_info, client_info;
	pthread_t t;


	// utwórz socket serwera
	server_socket = socket(AF_INET, SOCK_STREAM, 0);
	server_info.sin_family = AF_INET;
	server_info.sin_addr.s_addr = INADDR_ANY;
	server_info.sin_port = htons(5555);
	bind(server_socket, (struct sockaddr*)&server_info, sizeof(server_info));
	listen(server_socket, 100);

	// akceptuj połączenie od kolejnego klienta...	
	while ((client_socket = accept(server_socket, (struct sockaddr*)&client_info, (socklen_t*)&c)) != -1)
	{
		//...i przetwórz je w osobnym wątku
		pthread_create(&t, 0, thread_function, &client_socket);
	}

	// resetuj socket
	shutdown(server_socket, 0);

	// koniec programu serwera
	return 0;
}
