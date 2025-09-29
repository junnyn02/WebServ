#include "serverCore.hpp"
#include "Request.hpp"
#include "ResponseBuilder.hpp"

// int main(int, char**) {
//     serverCore serv;
//     serv.startServer();
    
//     while (1) {
//         clientData data = serv.receiveRequest();
//         Request fresh(data);
//         ResponseBuilder response(fresh);
//         fresh.printRequest();
        
//         std::string hello = response.exec();
        
//         // 🔥 FORCER Connection: close dans toutes les réponses
//         // Vérifier si la réponse contient déjà Connection: close
//         if (hello.find("Connection: close") == std::string::npos) {
//             // L'ajouter avant le double CRLF
//             size_t headerEnd = hello.find("\r\n\r\n");
//             if (headerEnd != std::string::npos) {
//                 hello.insert(headerEnd, "\r\nConnection: close");
//             }
//         }
        
//         if (hello.length() >= BUFFER_SIZE) {
//             close(data.clientSocket);
//             continue;
//         }
        
//         // Envoyer et fermer immédiatement
//         send(data.clientSocket, hello.c_str(), hello.length(), 0);
//         close(data.clientSocket);
        
//         std::cout << "Connection closed, waiting for next client..." << std::endl;
//     }
    
//     return (0);
// }

int	main(int, char**)
{
	serverCore serv;

	serv.startServer();

// 	while (1) {
//     // 🔥 Déclarer data DANS la boucle pour qu'il soit réinitialisé à chaque itération
//     clientData data = serv.receiveRequest();
    
//     Request fresh(data);
//     ResponseBuilder response(fresh);
//     fresh.printRequest(); // remove later
    
//     std::string hello = response.exec(); // Plus propre
    
//     // Vérification de taille pour éviter buffer overflow
//     if (hello.length() >= BUFFER_SIZE) {
//         // Gérer l'erreur
//         continue;
//     }
    
//     // Copier la réponse
//     memcpy(data.buffer, hello.c_str(), hello.length());
//     memset(data.buffer + hello.length(), 0, BUFFER_SIZE - hello.length());
    
//     data.size = hello.length();
//     serv.sendResponse(data);
// }
	while (1)
	{
		clientData data = serv.receiveRequest();
		Request fresh(data);
		ResponseBuilder	response(fresh);
		fresh.printRequest(); // remove later
		std::string hello = response.sendResponse();
		for (size_t i = 0; i < hello.length(); i++)
			data.buffer[i] = hello.c_str()[i];
		for (size_t i = hello.length(); i < BUFFER_SIZE; i++)
			data.buffer[i] = 0;
		data.size = hello.length();
		serv.sendResponse(data);
		memset(&data, 0, sizeof(clientData));
	}
	return (0);
}