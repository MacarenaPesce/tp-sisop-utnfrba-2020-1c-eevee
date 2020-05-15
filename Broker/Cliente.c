#include <Pokebola.h>

int main(){

	t_packed *paquete;
	paquete = (t_packed*)malloc(sizeof(t_packed));

	int socket =  conectar_a_server("127.0.0.1","6009");

	enviar_appeared_pokemon(socket, -1, -1, "pikachu",10,20);

}
