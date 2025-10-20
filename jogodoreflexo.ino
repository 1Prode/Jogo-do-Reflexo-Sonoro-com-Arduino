// ================== TIPOS ==================
// Cada estado representa uma fase diferente do funcionamento do sistema.
// A transição entre eles acontece com base no tempo (millis) ou ações do jogador.
enum Estado {
  ESPERANDO_INICIO, PREPARACAO, ESPERA_ALEATORIA,
  SINAL_CORRETO, SINAL_ERRADO, POS_SINAL,
  FEEDBACK, VOLTAR_INICIO
};

// Tipos possíveis de feedback após cada rodada
enum TipoFeedback { SUCESSO, ERRO, TEMPO_ESGOTADO };

// ============= PROTÓTIPOS =============
void iniciarFeedback(TipoFeedback tipo);
void loopFeedback();
void tocarSom(int freq, unsigned long duracao);
void atualizarSom();
void lerBotoes();
void acenderLeds(bool vermelho, bool amarelo, bool verde);

// ================== PINOS ================== 
const int ledVermelho = 8;
const int ledAmarelo  = 9;
const int ledVerde    = 10;
const int botaoInicio = 12;
const int botaoAcao   = 13;
const int buzzer      = 11;
const int pinoRuido   = A0;    // pino flutuante usado como fonte de ruído para randomSeed()

// ================== TEMPOS =================
// Aqui definimos os tempos usados pelo millis() para controlar as transições.
const unsigned long tempoEtapaPrep = 250;     // tempo entre as trocas R -> Y -> G
const unsigned long atrasoMinimo   = 2000;    // 2 s (mínimo para gerar atraso aleatório)
const unsigned long atrasoMaximo   = 5000;    // 5 s (máximo)

// Sinal correto: 3 bipes curtos
const unsigned long tempoSomCurtoLigado  = 200;
const unsigned long tempoSomCurtoDesligado = 90;
const int frequenciaSomCorreto = 1200;

// Sinal "armadilha": 1 bip longo
const unsigned long tempoSomLongo = 1000;
const int frequenciaSomErrado = 400;

// Feedback: piscar 5x + som
const int  qtdPiscadasFeedback   = 5;
const unsigned long tempoLedLigadoFeedback  = 150;
const unsigned long tempoLedDesligadoFeedback = 120;
const int  somSucesso   = 1500;
const int  somErro      = 350;
const int  somTimeout   = 700;

// Debounce simples (para leitura estável dos botões)
const unsigned long tempoDebounce = 25;

// ================== VARIÁVEIS GLOBAIS ==================
// Essas variáveis guardam o tempo atual e o tempo do próximo evento.
// Elas são comparadas com millis() para saber quando mudar de estado.
unsigned long agora = 0, proximoTempo = 0, tempoEtapa = 0;

int etapaPreparacao = 0;
bool padraoCorreto = false;
bool somLigado = false;
int  qtdBipes = 0;

// Estados dos botões com debounce
int leituraBrutaInicio = LOW, leituraBrutaAcao = LOW;
int estadoInicio = LOW, estadoAcao = LOW;
unsigned long tempoMudancaInicio = 0, tempoMudancaAcao = 0;

// Estado inicial do sistema
Estado estadoAtual = ESPERANDO_INICIO;

// Controle do som
bool somAtivo = false;
unsigned long tempoSomAte = 0;

// Controle do feedback
int  pinoFeedback = ledVerde;
TipoFeedback tipoFeedback = SUCESSO;
bool ledFeedbackLigado = false;
int  contadorFeedback = 0;
unsigned long proximoFeedback = 0;

// ================== FUNÇÕES AUXILIARES ==================

// Inicia o som por um tempo específico, sem bloquear o código.
// O controle é feito com millis(): enquanto millis() < tempoSomAte, o som toca.
void tocarSom(int freq, unsigned long duracao) {
  if (freq <= 0 || duracao == 0) {
    noTone(buzzer);
    somAtivo = false;
    return;
  }
  tone(buzzer, freq);
  somAtivo = true;
  tempoSomAte = millis() + duracao; // define quando o som deve parar
}

// Para o som automaticamente quando o tempo definido passa.
void atualizarSom() {
  if (somAtivo && millis() >= tempoSomAte) {
    noTone(buzzer);
    somAtivo = false;
  }
}

// Lê os botões com debounce simples usando millis()
// Isso evita leituras falsas sem travar o loop com delay()
void lerBotoes() {
  int leituraInicio = digitalRead(botaoInicio);
  int leituraAcao   = digitalRead(botaoAcao);
  unsigned long agora = millis();

  if (leituraInicio != leituraBrutaInicio) {
    tempoMudancaInicio = agora;
    leituraBrutaInicio = leituraInicio;
  }
  if (leituraAcao != leituraBrutaAcao) {
    tempoMudancaAcao = agora;
    leituraBrutaAcao = leituraAcao;
  }

  if (agora - tempoMudancaInicio > tempoDebounce) estadoInicio = leituraInicio;
  if (agora - tempoMudancaAcao > tempoDebounce)  estadoAcao   = leituraAcao;
}

// Liga/desliga os LEDs conforme o estado atual
void acenderLeds(bool vermelho, bool amarelo, bool verde) {
  digitalWrite(ledVermelho, vermelho);
  digitalWrite(ledAmarelo, amarelo);
  digitalWrite(ledVerde, verde);
}

// Inicia o feedback (LED piscando + som)
// Usa millis() para alternar os intervalos de luz e som.
void iniciarFeedback(TipoFeedback tipo) {
  noTone(buzzer);
  somAtivo = false;
  acenderLeds(false, false, false);

  tipoFeedback = tipo;
  pinoFeedback = (tipo == SUCESSO) ? ledVerde :
                 (tipo == ERRO) ? ledVermelho : ledAmarelo;

  // Primeira piscada imediata
  ledFeedbackLigado = true;
  digitalWrite(pinoFeedback, HIGH);

  int freq = (tipo == SUCESSO) ? somSucesso :
             (tipo == ERRO) ? somErro : somTimeout;
  tocarSom(freq, tempoLedLigadoFeedback);

  contadorFeedback = 1;
  proximoFeedback = millis() + tempoLedLigadoFeedback;
  estadoAtual = FEEDBACK;
}

// Mantém o loop do feedback funcionando sem delay()
void loopFeedback() {
  unsigned long agora = millis();

  if (contadorFeedback >= qtdPiscadasFeedback) {
    digitalWrite(pinoFeedback, LOW);
    noTone(buzzer);
    somAtivo = false;
    estadoAtual = VOLTAR_INICIO;
    proximoTempo = agora + 200;
    return;
  }

  // Alterna LED ligado/desligado de acordo com o tempo usando millis()
  if (agora >= proximoFeedback) {
    ledFeedbackLigado = !ledFeedbackLigado;
    digitalWrite(pinoFeedback, ledFeedbackLigado ? HIGH : LOW);

    if (ledFeedbackLigado) {
      int freq = (tipoFeedback == SUCESSO) ? somSucesso :
                 (tipoFeedback == ERRO) ? somErro : somTimeout;
      tocarSom(freq, tempoLedLigadoFeedback);
      contadorFeedback++;
      proximoFeedback = agora + tempoLedLigadoFeedback;
    } else {
      noTone(buzzer);
      somAtivo = false;
      proximoFeedback = agora + tempoLedDesligadoFeedback;
    }
  }
}

// ================== SETUP ==================
void setup() {
  pinMode(ledVermelho, OUTPUT);
  pinMode(ledAmarelo, OUTPUT);
  pinMode(ledVerde, OUTPUT);
  pinMode(buzzer, OUTPUT);
  pinMode(botaoInicio, INPUT);
  pinMode(botaoAcao, INPUT);

  // Evita leituras falsas ao ligar
  leituraBrutaInicio = digitalRead(botaoInicio);
  leituraBrutaAcao   = digitalRead(botaoAcao);
  estadoInicio = leituraBrutaInicio;
  estadoAcao   = leituraBrutaAcao;

  // Gera semente aleatória com leitura de ruído de um pino flutuante
  randomSeed(analogRead(pinoRuido));

  acenderLeds(true, false, false);
}

// ================== LOOP PRINCIPAL ==================
void loop() {
  // millis() serve como relógio global. É usado em todas as comparações de tempo.
  agora = millis();

  // Atualiza botões e sons continuamente
  lerBotoes();
  atualizarSom();

  // Estrutura de máquina de estados — muda o comportamento conforme o estadoAtual.
  switch (estadoAtual) {

    case ESPERANDO_INICIO:
      acenderLeds(true, false, false);
      if (estadoInicio == HIGH) {
        etapaPreparacao = 0;
        proximoTempo = agora;
        estadoAtual = PREPARACAO;
      }
      break;

    case PREPARACAO:
      // Usa millis() para acionar cada cor de LED por um tempo definido
      if (agora >= proximoTempo) {
        if (etapaPreparacao == 0) {
          acenderLeds(true, false, false);
          proximoTempo = agora + tempoEtapaPrep;
          etapaPreparacao++;
        } else if (etapaPreparacao == 1) {
          acenderLeds(false, true, false);
          proximoTempo = agora + tempoEtapaPrep;
          etapaPreparacao++;
        } else if (etapaPreparacao == 2) {
          acenderLeds(false, false, true);
          proximoTempo = agora + tempoEtapaPrep;
          etapaPreparacao++;
        } else {
          // Define um tempo aleatório de espera usando random()
          unsigned long atraso = random(atrasoMinimo, atrasoMaximo + 1);
          acenderLeds(false, false, true);
          proximoTempo = agora + atraso;
          estadoAtual = ESPERA_ALEATORIA;
        }
      }
      break;

    case ESPERA_ALEATORIA:
      // Se o jogador apertar antes do sinal, gera timeout
      if (estadoAcao == HIGH) {
        iniciarFeedback(TEMPO_ESGOTADO);
        break;
      }

      // Quando o tempo aleatório se esgota, escolhe sinal correto ou errado
      if (agora >= proximoTempo) {
        padraoCorreto = (random(0, 2) == 1);
        if (padraoCorreto) {
          acenderLeds(false, false, true);
          qtdBipes = 0;
          somLigado = false;
          tempoEtapa = agora;
          estadoAtual = SINAL_CORRETO;
        } else {
          acenderLeds(true, false, false);
          tocarSom(frequenciaSomErrado, tempoSomLongo);
          proximoTempo = agora + tempoSomLongo;
          estadoAtual = SINAL_ERRADO;
        }
      }
      break;

    case SINAL_CORRETO:
      // Durante o sinal correto, alterna som ON/OFF controlado por millis()
      if (estadoAcao == HIGH) {
        iniciarFeedback(SUCESSO);
        break;
      }
      if (agora >= tempoEtapa) {
        somLigado = !somLigado;
        if (somLigado) {
          tocarSom(frequenciaSomCorreto, tempoSomCurtoLigado);
          tempoEtapa = agora + tempoSomCurtoLigado;
        } else {
          noTone(buzzer);
          tempoEtapa = agora + tempoSomCurtoDesligado;
          qtdBipes++;
          if (qtdBipes >= 3) {
            acenderLeds(false, false, false);
            proximoTempo = agora + 500;
            estadoAtual = POS_SINAL;
          }
        }
      }
      break;

    case SINAL_ERRADO:
      if (estadoAcao == HIGH) {
        iniciarFeedback(ERRO);
        break;
      }
      if (agora >= proximoTempo) {
        noTone(buzzer);
        acenderLeds(false, false, false);
        proximoTempo = agora + 500;
        estadoAtual = POS_SINAL;
      }
      break;

    case POS_SINAL:
      if (estadoAcao == HIGH) {
        iniciarFeedback(TEMPO_ESGOTADO);
      } else if (agora >= proximoTempo) {
        estadoAtual = VOLTAR_INICIO;
        proximoTempo = agora + 200;
      }
      break;

    case FEEDBACK:
      loopFeedback();
      break;

    case VOLTAR_INICIO:
      if (agora >= proximoTempo) {
        acenderLeds(true, false, false);
        estadoAtual = ESPERANDO_INICIO;
      }
      break;
  }
}