#include <SFML/Graphics.hpp>
#include <iostream>
#include <ctime>
#include <cstdlib>

enum EstadoJuego {
    MENU, 
    PARTIDA
};

enum BotonAccion {
    NINGUNA,
    PEDIR_CARTA,
    PLANTARSE, 
    INICIAR
};

struct Boton {
    sf::RectangleShape shape;
    bool isOver = false;
    bool isPressedInside = false;
    bool isMousePressed = false;
};

BotonAccion manejarBoton(Boton& boton, sf::Vector2f mouse_position, BotonAccion accion);
bool randRepetido(int rand[], int cantidad, int valor);
int ajustarAs(int total, int cartas[], int valorCartas[], int cantidad);

// main del programa
int main()
{
    //semilla para numeros aleatorios 
    srand(time(0));
    int randJugador[5];
    int randMaquina[5];
    int valorCartas[53];
    int totalJugador; 
    int totalMaquina;
    // creamos la ventana
    sf::RenderWindow window(sf::VideoMode({900, 600}), "Title");
    // sacamos el tamaño de la ventana para acomodar botones imagenes, etc.
    sf::Vector2u tamanioVentana = window.getSize();

    // string
    std::string string = "BLACK JACK\n\nBIENVENIDO\n\nPresiona el boton para iniciar";
    sf::Font font("C:/Windows/Fonts/arial.ttf");
    sf::Text text(font);
    text.setCharacterSize(50);
    text.setLetterSpacing(1.5f);

    // Cargamos imagen de ganaste o perdiste 
    sf::Texture ganaste("Cartas/ganaste.png");
    ganaste.setSmooth(true);
    sf::Sprite gan(ganaste);
    sf::FloatRect boundsganaste = gan.getLocalBounds();
    gan.setOrigin(boundsganaste.size.componentWiseMul({0.5f, 0.9f}));
    gan.setPosition({400.f, 300.f});
    gan.setScale({0.4f, 0.4f});

    sf::Texture perdiste("Cartas/perdiste.png");
    perdiste.setSmooth(true);
    sf::Sprite per(perdiste);
    sf::FloatRect boundsperdiste = per.getLocalBounds();
    per.setOrigin(boundsperdiste.size.componentWiseMul({0.5f, 0.9f}));
    per.setPosition({400.f, 300.f});
    per.setScale({0.4f, 0.4f});

    sf::Texture empate("Cartas/empate.png");
    empate.setSmooth(true);
    sf::Sprite emp(empate);
    sf::FloatRect boundsempate = emp.getLocalBounds();
    emp.setOrigin(boundsempate.size.componentWiseMul({0.5f, 0.9f}));
    emp.setPosition({400.f, 300.f});
    emp.setScale({0.4f, 0.4f});

    // Vectores para almacenar las texturas y los sprites de las cartas
    std::vector<sf::Texture> cardTextures;
    std::vector<sf::Sprite> cardSprites;

    // Cargamos las imagenes de las cartas desde un vector para poder manipular todas 
    for (int i = 0; i <= 53; ++i) {
        std::string filename = "Cartas/imagen" + std::to_string(i) + ".jpg";
        sf::Texture texture;
        if (texture.loadFromFile(filename)) {
            cardTextures.push_back(texture);
        } else {
            std::cout << "Error al cargar la imagen: " << filename << std::endl;
        }
    }

    // Crear los sprites a partir de las texturas cargadas 
    for (const auto& texture : cardTextures) {
        sf::Sprite sprite(texture);
        sf::FloatRect bounds = sprite.getLocalBounds();
        sprite.setOrigin(bounds.size.componentWiseMul({0.5f, 0.5f}));//Centra el sprite
        sprite.setScale({0.4f, 0.4f});//Tamaño de las imagenes
        cardSprites.push_back(sprite);//Guarda el sprite en el vector
    }

    
    //Aqui se le asigna los valores de las cartas para poder hacer la logica de juego 
    for (int i = 1; i <= 52; ++i) {
        int pos = (i - 1) % 13; 
        if (pos <= 8) {
            valorCartas[i] = pos + 2; 
        } else if (pos <= 11) {
            valorCartas[i] = 10;      
        } else {
            valorCartas[i] = 11;
        }
    }
    
    //ciclo para las cartas randmon del jugador 
    int i=0;
    while (i < 5) {
        int num = 1 + rand() % 52;
        if (!randRepetido(randJugador, i, num)) {
            randJugador[i] = num;
            i++;
        }
    }
    //ciclo para las cartas random de la maquina 
    i = 0;
    while (i < 5) {
        int num = 1 + rand() % 52;
        if (!randRepetido(randJugador, 5, num) && !randRepetido(randMaquina, i, num)) {
            randMaquina[i] = num;
            i++;
        }
    }

    // Aqui acomodamos la posicion de las imagenes en la pantalla 
    // Carta que simula la baraja 
    cardSprites[0].setPosition({tamanioVentana.x * .10f, tamanioVentana.y * .50f});

    //Cartas aleatorias del jugador 
    cardSprites[randJugador[0]].setPosition({tamanioVentana.x * .45f, tamanioVentana.y * .65f});
    cardSprites[randJugador[1]].setPosition({tamanioVentana.x * .58f, tamanioVentana.y * .65f});
    cardSprites[randJugador[2]].setPosition({tamanioVentana.x * .66f, tamanioVentana.y * .65f});
    cardSprites[randJugador[3]].setPosition({tamanioVentana.x * .79f, tamanioVentana.y * .65f});
    cardSprites[randJugador[4]].setPosition({tamanioVentana.x * .92f, tamanioVentana.y * .65f});

    //Cartas aleatorias de la maquina 
    cardSprites[randMaquina[0]].setPosition({tamanioVentana.x * .45f, tamanioVentana.y * .15f});
    cardSprites[randMaquina[1]].setPosition({tamanioVentana.x * .58f, tamanioVentana.y * .15f});
    cardSprites[randMaquina[2]].setPosition({tamanioVentana.x * .66f, tamanioVentana.y * .15f});
    cardSprites[randMaquina[3]].setPosition({tamanioVentana.x * .79f, tamanioVentana.y * .15f});
    cardSprites[randMaquina[4]].setPosition({tamanioVentana.x * .92f, tamanioVentana.y * .15f});
    cardSprites[53].setPosition({tamanioVentana.x * .58f, tamanioVentana.y * .15f});

    // creacion de botones 
    Boton botonPedir;
    botonPedir.shape.setSize({120.f, 40.f});
    botonPedir.shape.setPosition({tamanioVentana.x * .60f, tamanioVentana.y * .90f});
    botonPedir.shape.setOrigin(botonPedir.shape.getSize() / 2.f);

    Boton botonPlantarse;
    botonPlantarse.shape.setSize({120.f, 40.f});
    botonPlantarse.shape.setPosition({tamanioVentana.x * .40f, tamanioVentana.y * .90f});
    botonPlantarse.shape.setOrigin(botonPlantarse.shape.getSize() / 2.f);

    Boton iniciar;
    iniciar.shape.setSize({120.f, 40.f});
    iniciar.shape.setPosition({tamanioVentana.x * .5f, tamanioVentana.y * .90f});
    iniciar.shape.setOrigin(iniciar.shape.getSize() / 2.f);

    bool mostrarImagen3 = false;
    bool mostrarImagen4 = false;
    bool mostrarImagen5 = false;
    bool mostrarGanaste = false;
    bool mostrarPerdiste = false;
    bool mostrarEmpate = false;
    int cont = 0;
    EstadoJuego estadoActual = MENU;
    sf::Clock relojCartasMaquina;
    bool mazoMaquina = false;
    int cartasMaquinaMostradas = 0;
    int contMaquinaAnimacion = 2; 
    
    // window loop
    while (window.isOpen())
    {
        // handle events
        while (auto event = window.pollEvent())
        {
            // on close button release
            if (event->is<sf::Event::Closed>()){
                // close window
                window.close();
            }
            // when window is resized
            else if (event->is<sf::Event::Resized>()){
                // update view
                window.setView(sf::View(sf::FloatRect({0.f, 0.f}, sf::Vector2f(window.getSize()))));
            }
        }

    //Mostramos el menu del juego 
    if (estadoActual == MENU)
    {
        auto mouse_position = sf::Vector2f(sf::Mouse::getPosition(window));
        BotonAccion accion3 = manejarBoton(iniciar, mouse_position, INICIAR);

        if (accion3 == INICIAR)
        {
            estadoActual = PARTIDA;
            totalJugador = valorCartas[randJugador[0]] + valorCartas[randJugador[1]];
            std::cout << "Valor : " << totalJugador << std::endl;
            totalMaquina = valorCartas[randMaquina[0]] + valorCartas[randMaquina[1]];
            std::cout << "Valor Maquina: " << totalMaquina << std::endl;
            if (totalJugador == 21 ){
                mostrarGanaste = true;
            }
        }

        window.clear(sf::Color(64, 64, 64));

        // Texto de menu
        int n = 0;
        for (int start = 0;; n++)
        {
            start = string.find('\n', start) + 1;
            if (start == 0)
                break;
        }
        for (int start = 0, i = 0;; i++)
        {
            int end = string.find('\n', start);

            text.setString(string.substr(start, end - start));
            sf::FloatRect bounds = text.getLocalBounds();
            text.setOrigin({bounds.position.x + bounds.size.x/2, text.getCharacterSize() * 0.75f});
            text.setPosition({tamanioVentana.x * 0.5f,tamanioVentana.y * 0.5f + (i - n * 0.5f) * 1.5f * text.getCharacterSize()});
            window.draw(text);

            if (end == -1)
                break;

            start = end + 1;
 
        }

        window.draw(iniciar.shape);
        window.display();

    }
    else if (estadoActual == PARTIDA)
    {

        // posicion del mouse 
        auto mouse_position = sf::Vector2f(sf::Mouse::getPosition(window));

        // Manejar botones
        BotonAccion accion1 = manejarBoton(botonPedir, mouse_position, PEDIR_CARTA);
        BotonAccion accion2 = manejarBoton(botonPlantarse, mouse_position, PLANTARSE);

        //Condicional del jugador 
        if (accion1 == PEDIR_CARTA)
        {
            if (cont < 3)
            {
                cardSprites[randJugador[0]].move({tamanioVentana.x * -.05f, 0.f});
                cardSprites[randJugador[1]].move({tamanioVentana.x * -.05f, 0.f});
              if (mostrarImagen3==true )
              {
                  cardSprites[randJugador[2]].move({tamanioVentana.x * -.05f, 0.f});
                  mostrarImagen4 = true;
              } 
              if (mostrarImagen4 == true ){
                  cardSprites[randJugador[3]].move({tamanioVentana.x * -.05f, 0.f});
                  mostrarImagen5 = true;
              }
              if (mostrarImagen5 == true ){
                 cardSprites[randJugador[4]].move({tamanioVentana.x * -.05f, 0.f});
              }

              mostrarImagen3 = true;
             
                // Aqui desarrolamos la logica del juego para saber si el jugador gano o perdio
                 int cartasEnJuego = 2 + cont + 1;
                 totalJugador = ajustarAs(0, randJugador, valorCartas, cartasEnJuego);
                 std::cout << "Valor total jugador: " << totalJugador << std::endl;
                 
            if (totalJugador > 21) 
            {
                relojCartasMaquina.restart(); 
               cartasMaquinaMostradas = 2;
               contMaquinaAnimacion = 2; 
               mazoMaquina = true;
               totalMaquina = ajustarAs(totalMaquina, randMaquina, valorCartas, cartasMaquinaMostradas);

               //Logica de la maquina para saber quien gana 
                while (totalMaquina < 17 && cartasMaquinaMostradas < 5) {
                    totalMaquina += valorCartas[randMaquina[cartasMaquinaMostradas]];
                    cartasMaquinaMostradas++;
                    totalMaquina = ajustarAs(totalMaquina, randMaquina, valorCartas, cartasMaquinaMostradas);
                }

                std::cout << "Valor total maquina: " << totalMaquina << std::endl;
                std::cout << "Maquina mostradas " << cartasMaquinaMostradas << std::endl;

                // Verificar ganador
                if (totalMaquina > 21){
                    mostrarEmpate = true;
                }else {
                    mostrarPerdiste = true;
                }        
            }
                if (cont == 2 && totalJugador <= 21) {
                    mostrarGanaste = true;
                }

              cont ++;
            }
        }        
        else if (accion2 == PLANTARSE)
        {
               relojCartasMaquina.restart(); 
               cartasMaquinaMostradas = 2;
               contMaquinaAnimacion = 2; 
               mazoMaquina = true;
               totalMaquina = ajustarAs(totalMaquina, randMaquina, valorCartas, cartasMaquinaMostradas);

               //Logica de la maquina para saber quien gana 
                while (totalMaquina < 17 && cartasMaquinaMostradas < 5) {
                    totalMaquina += valorCartas[randMaquina[cartasMaquinaMostradas]];
                    cartasMaquinaMostradas++;
                    totalMaquina = ajustarAs(totalMaquina, randMaquina, valorCartas, cartasMaquinaMostradas);
                }

                std::cout << "Valor total maquina: " << totalMaquina << std::endl;
                std::cout << "Maquina mostradas " << cartasMaquinaMostradas << std::endl;

                // Verificar ganador
                if (totalMaquina > 21 && totalJugador > 21){
                    mostrarEmpate = true;
                }else if (totalMaquina > 21) {
                    mostrarGanaste = true;
                } else if (totalMaquina >= totalJugador) {
                    mostrarPerdiste = true;
                } else {
                    mostrarGanaste = true;
                }            

        }

        window.clear(sf::Color(64, 64, 64));
        window.draw(botonPedir.shape);
        window.draw(botonPlantarse.shape);
        
        // Llamamos a las cartas para ser dibujadas 
        window.draw(cardSprites[0]);
        window.draw(cardSprites[randJugador[0]]);
        window.draw(cardSprites[randJugador[1]]);
        if (mostrarImagen3){
            window.draw(cardSprites[randJugador[2]]);
        }
        if (mostrarImagen4){
            window.draw(cardSprites[randJugador[3]]);
        }
        if (cont == 3){
            window.draw(cardSprites[randJugador[4]]);
        }

        if (!mazoMaquina) {
            window.draw(cardSprites[randMaquina[0]]);
            window.draw(cardSprites[53]);
        }
        
        //logica del juego de la maquina
        if (mazoMaquina) {
            sf::Time tiempo = relojCartasMaquina.getElapsedTime();

            if (contMaquinaAnimacion < cartasMaquinaMostradas && tiempo.asSeconds() > 1.5f * contMaquinaAnimacion) {
                int index = contMaquinaAnimacion;
                if (index == 2){
                    cardSprites[randMaquina[0]].move({tamanioVentana.x * -0.05f, 0.f});
                    cardSprites[randMaquina[1]].move({tamanioVentana.x * -0.05f, 0.f});
                }
                if (index > 2) {
                    cardSprites[randMaquina[0]].move({tamanioVentana.x * -0.05f, 0.f});
                    cardSprites[randMaquina[1]].move({tamanioVentana.x * -0.05f, 0.f});
                    cardSprites[randMaquina[2]].move({tamanioVentana.x * -0.05f, 0.f});
                    cardSprites[randMaquina[3]].move({tamanioVentana.x * -0.05f, 0.f});
                    cardSprites[randMaquina[4]].move({tamanioVentana.x * -0.05f, 0.f});
                }
                contMaquinaAnimacion++;
            }
        
             for (int i = 0; i < contMaquinaAnimacion; ++i) {
                window.draw(cardSprites[randMaquina[i]]);
            }
        }
        if (mostrarGanaste){
            window.draw(gan);
        }
        if (mostrarPerdiste){
            window.draw(per);
        }
        if (mostrarEmpate){
                window.draw(emp);
        }

        // display
        window.display();
    
    }
  }

  return 0;
}

BotonAccion manejarBoton(Boton& boton, sf::Vector2f mouse_position, BotonAccion accion)
{
 
     // estado el boton 
     bool onMousePress   = false;
     bool onMouseRelease = false;

     if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
     {
         if (not boton.isMousePressed)
             onMousePress = true;
             boton.isMousePressed = true;
     }
     else
     {
         if (boton.isMousePressed)
             onMouseRelease = true;
             boton.isMousePressed = false;
     }

     if (boton.shape.getGlobalBounds().contains(mouse_position))
     {
     
         // al presionar el mouse 
         if (onMousePress)
         {
             boton.isPressedInside = true;
         }

         // al soltar el raton 
         if (onMouseRelease and boton.isPressedInside)
         {
             return accion;
         }

         // cuando se presiona el mouse 
         if (boton.isMousePressed and boton.isPressedInside)
             boton.shape.setFillColor(sf::Color(0, 170, 255));

         // estado
         boton.isOver = true;
     }
     else
     {
         boton.shape.setFillColor(sf::Color::White);
         // reiniciar estado 
         boton.isOver = false;
     }
     // si se presiono el reinicio dentro
     if (not boton.isMousePressed)
         boton.isPressedInside = false;

         return NINGUNA;
}

bool randRepetido(int rand[], int cantidad, int valor){
      for (int i = 0; i < cantidad; i++) {
             if (rand[i] == valor){
                return true;
               }
        }
    return false;
}

int ajustarAs(int total,int cartas[],int valorCartas[], int cantidad) {
    int asesSinAjustar = 0;
    int tempTotal = 0;

    for (int i = 0; i < cantidad; ++i) {
        int valor = valorCartas[cartas[i]];
        tempTotal += valor;
        if (valor == 11) {
            asesSinAjustar++;
        }
    }

    while (tempTotal > 21 && asesSinAjustar > 0) {
        tempTotal -= 10;
        asesSinAjustar--;
    }

    return tempTotal;
}
