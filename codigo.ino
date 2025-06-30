#include <IRremote.hpp>

#define IR_PIN 11
#define LED_R 9   // LED sucesso
#define LED_W 8   // LED erro
#define LED_D0 2  // LEDs de progresso
#define LED_D1 3
#define LED_D2 4
#define LED_D3 5

// Códigos IR dos botões
#define K0 4278214279
#define K1 4261502599
#define K2 4244790919
#define K3 4228079239
#define K4 4211367559
#define K5 4194655879
#define K6 4177944199
#define K7 4161232519
#define K8 4144520839
#define K9 4127809159
#define KDEL 3826998919
#define KOK 3927268999

/*
0: 4278214279
1: 4261502599
2: 4244790919
3: 4228079239
4: 4211367559
5: 4194655879
6: 4177944199
7: 4161232519
8: 4144520839
9: 4127809159
esquerda: 3826998919
OK: 3927268999
*/

// Senha padrão: 1234
char password[4] = {'1', '2', '3', '4'};
bool hold = false;
bool updating_password = false;

void setup() {
    IrReceiver.begin(IR_PIN);

    pinMode(LED_R, OUTPUT);
    pinMode(LED_W, OUTPUT);
    pinMode(LED_D0, OUTPUT);
    pinMode(LED_D1, OUTPUT);
    pinMode(LED_D2, OUTPUT);
    pinMode(LED_D3, OUTPUT);
}

char current_pass[5];
int stack_counter = 0;

// Converte sinal IR em tecla
char get_key() {
    if (IrReceiver.decodedIRData.decodedRawData == K0) return '0';
    if (IrReceiver.decodedIRData.decodedRawData == K1) return '1';
    if (IrReceiver.decodedIRData.decodedRawData == K2) return '2';
    if (IrReceiver.decodedIRData.decodedRawData == K3) return '3';
    if (IrReceiver.decodedIRData.decodedRawData == K4) return '4';
    if (IrReceiver.decodedIRData.decodedRawData == K5) return '5';
    if (IrReceiver.decodedIRData.decodedRawData == K6) return '6';
    if (IrReceiver.decodedIRData.decodedRawData == K7) return '7';
    if (IrReceiver.decodedIRData.decodedRawData == K8) return '8';
    if (IrReceiver.decodedIRData.decodedRawData == K9) return '9';
    if (IrReceiver.decodedIRData.decodedRawData == KDEL) return 'D';
    if (IrReceiver.decodedIRData.decodedRawData == KOK) return 'O';

    return '\0';
}

// Atualiza LEDs de progresso
void update_leds(int n) {
    digitalWrite(LED_D0, LOW);
    digitalWrite(LED_D1, LOW);
    digitalWrite(LED_D2, LOW);
    digitalWrite(LED_D3, LOW);

    if (n >= 1) digitalWrite(LED_D0, HIGH);
    if (n >= 2) digitalWrite(LED_D1, HIGH);
    if (n >= 3) digitalWrite(LED_D2, HIGH);
    if (n >= 4) digitalWrite(LED_D3, HIGH);
}

// Armazena tecla digitada
void insert_key(char key) {
    if (key != '\0') {
        current_pass[stack_counter] = key;
        stack_counter++;
        update_leds(stack_counter);
    }
}

// Confere se senha bate
bool check_pass() {
    bool correct_pass = true;
    
    for(int i = 0; i < 4; i++) {
        if(current_pass[i] != password[i]) correct_pass = false;
    }

    return correct_pass;
}

// Animação de senha correta
void correct_password() {
    digitalWrite(LED_R, HIGH);
    
    update_leds(0);
    delay(200);
    update_leds(1);
    delay(200);
    update_leds(2);
    delay(200);
    update_leds(3);
    delay(200);
    update_leds(4);
    delay(200);

    update_leds(0);
    delay(400);
    update_leds(4);
    delay(400);
    update_leds(0);
    delay(400);
    update_leds(4);
    delay(400);
    update_leds(0);

    digitalWrite(LED_R, LOW);

    stack_counter = 0;
}

// Animação de senha errada
void incorrect_password() {
    update_leds(0);
    
    for(int i = 500; i >= 50; i -= 50) {
        digitalWrite(LED_W, HIGH);
        delay(i);
        digitalWrite(LED_W, LOW);
        delay(i);
    }

    digitalWrite(LED_W, HIGH);

    stack_counter = 0;

    update_leds(4);

    delay(3000);

    update_leds(0);
    digitalWrite(LED_W, LOW);
}

bool last_was_correct = false;

void loop() {
    if (IrReceiver.decode() && !hold) {
        hold = true;
        
        char key = get_key();

        // Se acertou a senha, OK ativa modo de alteração de senha
        if(last_was_correct && key == 'O' && !updating_password) {
            stack_counter = 0;
            updating_password = true;
            digitalWrite(LED_W, HIGH);
            digitalWrite(LED_R, HIGH);
            update_leds(stack_counter);

            delay(200);
            IrReceiver.resume();
            return;
        }
        
        if(updating_password) {
            if(key == 'O') {
                // Sai do modo troca
                stack_counter = 0;
                updating_password = false;
                digitalWrite(LED_W, LOW);
                digitalWrite(LED_R, LOW);
                update_leds(stack_counter);

                IrReceiver.resume();
                return;
            }

            if(key == 'D') {
                // Deleta último dígito
                if(stack_counter != 0) {
                    stack_counter--;
                    update_leds(stack_counter);
                } 
            } else {
                insert_key(key);
            }

            if(stack_counter == 4) {
                // Salva nova senha
                password[0] = current_pass[0];
                password[1] = current_pass[1];
                password[2] = current_pass[2];
                password[3] = current_pass[3];

                stack_counter = 0;
                updating_password = false;

                delay(200);
                digitalWrite(LED_R, LOW);
                digitalWrite(LED_W, LOW);
                update_leds(0);
                delay(200);
                digitalWrite(LED_R, HIGH);
                digitalWrite(LED_W, HIGH);
                update_leds(4);
                delay(200);
                digitalWrite(LED_R, LOW);
                digitalWrite(LED_W, LOW);
                update_leds(0);
                delay(200);
                digitalWrite(LED_R, HIGH);
                digitalWrite(LED_W, HIGH);
                update_leds(4);
                delay(200);
                digitalWrite(LED_R, LOW);
                digitalWrite(LED_W, LOW);
            }

            update_leds(stack_counter);

            delay(200);
            IrReceiver.resume();
            return;
        }

        // Fora do modo troca
        if(key == 'D') {
            // Deleta último dígito
            if(stack_counter != 0) {
                stack_counter--;
                update_leds(stack_counter);
            } 
        } else if(key != 'O') {
            insert_key(key);
        }
        
        if(stack_counter == 4) {
            bool correct = check_pass();

            if(correct) {
                correct_password();
                last_was_correct = true;
            } else {
                incorrect_password();
                last_was_correct = false;
            }
        }

        IrReceiver.resume();
    } else {
        hold = false;
    }
    
    delay(200);
}

