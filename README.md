Projeto com finalidade de estudo.

##### O projeto abrange o uso e implementação de módulos para utilizar as seguintes tecnologias no chip esp-wroom-32:
- MQTT
- WIFI
- Display TFT
- QR Code
  
Comportamento do projeto:

1. Ao iniciar, um menu é apresentado, com os itens "MQTT, Brilho e WIFI".
2. Ao clicar em WIFI, é iniciado o AP da esp32 e mostrado um QR Code com as credênciais para conexão.
3. Ao conectar, a esp reconhece automaticamente, e muda o QR Code para apontar para a rota / do servidor interno.
4. Na página principal, é apresentado um formulário simples, com as redes wifi disponiveis, nivel de sinal e um campo para a senha.
5. Quando o usuário passar as credênciais para a esp, a senha é guardada na memória, a interface retorna para o menu principal e a esp se conecta ào broker MQTT, enviando qual seu nivel de brilho atual e uma mensagem amigável.
6. Ao receber um sinal de 0 a 255 no tópico `esp/control/bright` a esp atualiza o nivel de brilho.
7. Ao clicar no item MQTT, a esp envia uma mensagem de teste ao broker.


https://github.com/user-attachments/assets/3ddc213c-13c7-4853-abfa-3d07ee0f7417

