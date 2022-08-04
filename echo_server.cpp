/* 
 * File: echoserver.cpp || Author: kagancansit
 * Information Sources: https://www.youtube.com/watch?v=CFe5LQOPdfk --- https://www.yusufsezer.com.tr/c-cpp-soket/ -- https://my.eng.utah.edu/~cs4400/concurrency.pdf
 * Created on August 2, 2022, 10:34 AM
 * 
 * --USAGE--
 * SERVER -> g++ -std=c++11 echo_server.cpp -o server -> ./server 5000
 * CLIENT -> nc localhost 5000
 */

#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>
#include <string>
#include <thread>

//İçerik aktarımı için ara bellek değer tanımı.
#define BUFF_LEN 1024

//Donanımın max. kaldırabileceği thread sayısı ve thread tanımı.
static int max_thread = std::thread::hardware_concurrency();
std::thread commthreads;
int thread_counter = 0;

//Fonksiyon Tanımları
int getPortNumber(int, char**);
void bindingOperations(int, sockaddr_in);
void listeningOperations(int, int);
int acceptingOperations(int);
void recvOperations(int, char[]);
void sendingOperations(int, char[]);
void commClients(int, int, int);
void threadOperations(int, int);

int main(int argc, char** argv) {

    //Port numarasının alınması.
    int portNumber = getPortNumber(argc, argv);
    
    //Haberleşme için port tanımı. --- Socket Fonk. -> 1.Par: AF_INET (IPv4), AF_INET6 (IPv6)  // 2.Par -> SOCK_STREAM (TCP), SOCK_DGRAM (UDP) // 3.Par -> Transfer Prot.
    int socketNum = socket(AF_INET, SOCK_STREAM, 0);
    
    // Sunucu ve client için socket ip ilişkilendirilmesi.
    struct sockaddr_in server_address;
    memset(&server_address, 0, sizeof (server_address));
    server_address.sin_family = AF_INET; //(IPv4)
    server_address.sin_port = htons(portNumber); //Htons bilgisayarlar arasında byte önceliklendirmesini haberleşme için düzenler.
    
    //--BIND-LISTEN-ACCEPT--
    bindingOperations(socketNum, server_address);
    
    //RECV -- SEND
    threadOperations(socketNum, portNumber);
    
    //Soketlerin Kapatılması -- Server'e bağlı hiç client kalmadığı durumlarda "threadOperations" içerisinde basit bir if ile break eklenerek aşağıdaki method kullanılabilir.
    //shutdown(socketNum, SHUT_RDWR);
    return 0;
}

//------------------------------------------------------------------------------

//Haberleşme için Server'ın kullacığı port numarasının alınması.
int getPortNumber(int argc, char** argv) {
    if (argc < 2) {
        std::cout << "Usage: " << argv[0] << " <Port Number>" << std::endl;
        return 0;
    }
    return atoi(argv[1]);
}

//Bind - Bağlanma İşlemi
void bindingOperations(int socketNum, sockaddr_in socketAddress) {
    if ((bind(socketNum, (struct sockaddr *) &socketAddress, sizeof (socketAddress))) < 0) {
        perror("- Bağlantı sağlanamadı.\n");
        exit(0);
    } else
        std::cout << "\nBaşarıyla socket'e bağlanıldı." << std::endl;
}

//Listen - Dinleme İşlemleri
void listeningOperations(int socketNum, int portNum) 
{
    //Hangi Port Numarası, kaç kişi için dinleneceğini belirtiyoruz. Fazla istek sonucu parametre sonrası kabul edilmez.
    if ((listen(socketNum, max_thread)) < 0)
    {
        perror("Socket dinleme başarısız. \n");
        exit(0);
    }
    //else
        //std::cout << "\nSunucu " << portNum << " numaralı portu dinliyor. Bağlantı bekleniyor." << std::endl; 
}

//Accept - Bağlantı Kabul İşlemleri
int acceptingOperations(int socketNum) 
{    
    //Bağlantı kurmak isteyen Client hakkındaki verileri accept verisi ile bir socket adresi struct'ı içerisine tanımlıyoruz.
    struct sockaddr_in client_address;
    memset(&client_address, 0, sizeof (client_address));
    socklen_t remote_addrlen = sizeof (client_address);
    
    int client_socket = accept(socketNum, (struct sockaddr *) &client_address, &remote_addrlen);
    if (client_socket < 0) // 1 Hata - 0 Client'in bağlantısı kesildi.
    {
        perror("Bağlantı kurulamadı.");
        exit(0);
    } else if (client_socket == 0) {
        perror("Client'in bağlantısı kesildi veya sonlandırıldı.");
        exit(0);
    }
    std::string client_ip = inet_ntoa(client_address.sin_addr); //inet_ntoa -> Bilgisayar adresini IPv4 olarak ondalık diziye dönüştürür.
    int remote_port = ntohs(client_address.sin_port);           //htons fonksiyonun tersini gerçekleştirir.
    std::cout << "Yeni bağlantı sağlandı. IP: " << client_ip << " Port:" << remote_port << std::endl;

    return client_socket;
}

//Recv - Client tarafından gönderilen verilen kaydedilmesi.
void recvOperations(int socketNum, char buffer[])
{
    int bytes_received = recv(socketNum, buffer, BUFF_LEN - 1, 0);
    if (bytes_received < 0) {
        perror("Veri alınamadı.\n");
        exit(0);
    }
    if (buffer[bytes_received - 1] == '\n') {
        buffer[bytes_received - 1] = 0;
    }
    std::cout << "Client Mesajı: \"" << buffer << "\"" << std::endl;
}

//Send - Sunucunun Client'e Mesaj Gönderim İşlemleri
void sendingOperations(int socketNum, char buffer[]) 
{
    std::string response = std::string(buffer) + " _Veri alındı\n\n";
    
    if ((send(socketNum, response.c_str(), response.length(), 0)) < 0) {
        perror("Cevap gönderilemedi.");
        exit(0);
    }
}

//Recv - Send Thread Alanı
void commClients(int socketNum, int client_socket, int thread_id)
{    
    
    std::cout << "Socket: " << socketNum << " Client_socket: " << client_socket << std::endl;
    char buffer[BUFF_LEN];
    
    while(1)
    {
        memset(buffer, 0, BUFF_LEN); //Client'in kullandığı ara belleği her bağlantı öncesi temizliyoruz.
        //--RECV--
        recvOperations(client_socket, buffer);
        
        //Eğer client tarafından 'exit' girilirse client socketini kapat. 
        if (std::string(buffer) == "exit")
        {
            std::cout << "\nClient çıkış işlemi gerçekleştirdi...\n" << std::endl;
            thread_counter--;
            shutdown(client_socket, SHUT_RDWR);
            break;
        }
        else
            //--SEND--
            sendingOperations(client_socket, buffer); //Alınan mesajın sonuna " _Veri alındı." ekleyerek Client'e geri döndürür.
    }
}

//Thread İşlemleri
void threadOperations(int socketNum, int portNumber)
{
    while(1)
    {
        listeningOperations(socketNum, portNumber);
    
        int client_socket = acceptingOperations(socketNum);

        //Thread Tanımları
        if(thread_counter < max_thread)
        {
            //RECV -- SEND
            std::thread commthreads(commClients, socketNum, client_socket, thread_counter);
            thread_counter++;
            commthreads.detach();  
        }
    }
}