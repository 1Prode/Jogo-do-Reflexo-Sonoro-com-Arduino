# 🎮 Jogo do Reflexo Sonoro com Arduino

Projeto interativo desenvolvido em **Arduino** que testa o tempo de reação do jogador usando **LEDs, buzzer e botões**.  
O sistema é controlado por uma **lógica de estados** e utiliza **`millis()`** para medir o tempo com precisão, sem o uso de `delay()` — garantindo respostas imediatas e funcionamento fluido.

## 🧠 Simulação no Tinkercad

Você pode visualizar o circuito funcionando neste link:  
👉 [Abrir projeto no Tinkercad](https://www.tinkercad.com/things/6Q60RMs4SJj/editel?returnTo=%2Fdashboard%2Fdesigns%2Fcircuits&sharecode=m9eEt35gdRKDnU15XUrHEOM4VYlVYqU15QRz42yo8h8)
---

## ⚙️ Objetivo

Criar um jogo simples e divertido que mede o **tempo de reação** do jogador, ao mesmo tempo em que demonstra o uso prático de **controle de tempo com `millis()`** e **máquina de estados** no Arduino.

---

## 🔩 Materiais Utilizados

- 1 × Arduino UNO (ou compatível)
- 3 × LEDs (vermelho, amarelo e verde)
- 3 × Resistores de 220 Ω (para os LEDs)
- 2 × Botões (Início e Ação)
- 2 × Resistores de 10 kΩ (pull-down para os botões)
- 1 × Buzzer piezoelétrico
- Jumpers e protoboard

---

## ⚡ Funcionamento Geral

O jogo acende LEDs e emite sons que indicam quando o jogador deve reagir.  
O desafio é **apertar o botão de ação no momento certo**, evitando reagir antes do sinal correto.

- **LEDs**: indicam o estado atual (vermelho, amarelo ou verde).  
- **Buzzer**: emite sons curtos ou longos que diferenciam acertos, erros e tempo esgotado.  
- **Botão Início**: começa o jogo.
- **Botão Ação**: usado pelo jogador para reagir.

---

## 🧠 Lógica de Estados

O código usa uma **lógica de estados**, onde cada fase do jogo representa um comportamento.  
As transições entre os estados acontecem conforme o tempo (`millis()`) ou as ações do jogador.

| Estado | Descrição |
|--------|------------|
| **ESPERANDO_INICIO** | O sistema aguarda o jogador apertar o botão de início. |
| **PREPARACAO** | LEDs acendem em sequência (vermelho → amarelo → verde) sinalizando o início. |
| **ESPERA_ALEATORIA** | Um tempo aleatório é gerado antes do próximo sinal. |
| **SINAL_CORRETO** | Emite três bipes curtos (sinal válido para reagir). |
| **SINAL_ERRADO** | Emite um bip longo (armadilha sonora). |
| **POS_SINAL** | Breve intervalo aguardando ação do jogador. |
| **FEEDBACK** | Pisca LEDs e toca sons indicando sucesso, erro ou tempo esgotado. |
| **VOLTAR_INICIO** | Retorna ao estado inicial para reiniciar o jogo. |

---

## ⏱️ Lógica de Temporização

O projeto **não usa `delay()`**, o que permite que o Arduino execute várias tarefas em paralelo.  
A função `millis()` fornece o tempo em milissegundos desde que o Arduino foi ligado.  
A cada etapa, o programa compara o tempo atual com o tempo do próximo evento:
