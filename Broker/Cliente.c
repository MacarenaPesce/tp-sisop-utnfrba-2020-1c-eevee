#include <Pokebola.h>

int main(){

	t_packed *paquete;
	paquete = (t_packed*)malloc(sizeof(t_packed));

	int socket =  conectar_a_server("127.0.0.1","6009");

	enviar_mensaje_char(socket, "hola");

}
