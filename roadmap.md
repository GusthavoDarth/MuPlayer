# Fase 1: Sistema de Arquivos
 [x] Finalizar a recursão de diretórios.

 [ ] Criar um "Scanner" que lê arquivos e popula a struct MusicInfo.

 [ ] Implementar um parser manual de cabeçalho WAV. extrair: Sample Rate, Bit Depth e Number of Channels.

# Fase 2: O Pipeline de Áudio (O Motor)

 [ ] Implementar o Circular Buffer (Buffer de Anel). O decodificador enche o buffer e a placa de som esvazia.

 [ ] Criar a interface da HAL para áudio. No Windows, você terá que pesquisar sobre a WinMM (WaveOut) ou usar a Raylib apenas como "ponte" para o som por enquanto.

 [ ] Implementar o controle de volume (matemática pura: multiplicar o valor da amostra por um float de 0.0 a 1.0).

# Fase 3: Metadados e Interface

 [ ] Criar um parser para as tags (ID3 para MP3 ou metadados de WAV).

 [ ] Desenhar a lógica da Playlist (Adicionar, Remover, Avançar).

 [ ] Criar a View (Visualização) que consome os dados da MusicInfo.

# Fase 4: Portabilidade (O Salto para o ESP32)

 [ ] Substituir as funções de leitura de arquivo (POSIX/C Standard) pelas funções da biblioteca do SD Card do ESP32.

 [ ] Mapear a saída de áudio para o protocolo I2S.
