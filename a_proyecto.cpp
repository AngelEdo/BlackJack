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

// main del programa
int main()
{
    //semilla para numeros aleatorios 
    srand(time(0));
    int randJugador[5];
    int randMaquina[5];
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
    cardSprites[randJugador[1]].setPosition({tamanioVentana.x * .55f, tamanioVentana.y * .65f});
    cardSprites[randJugador[2]].setPosition({tamanioVentana.x * .60f, tamanioVentana.y * .65f});
    cardSprites[randJugador[3]].setPosition({tamanioVentana.x * .70f, tamanioVentana.y * .65f});
    cardSprites[randJugador[4]].setPosition({tamanioVentana.x * .80f, tamanioVentana.y * .65f});

    //Cartas aleatorias de la maquina 
    cardSprites[randMaquina[0]].setPosition({tamanioVentana.x * .45f, tamanioVentana.y * .15f});
    cardSprites[randMaquina[1]].setPosition({tamanioVentana.x * .55f, tamanioVentana.y * .15f});
    cardSprites[randMaquina[2]].setPosition({tamanioVentana.x * .60f, tamanioVentana.y * .15f});
    cardSprites[randMaquina[3]].setPosition({tamanioVentana.x * .70f, tamanioVentana.y * .15f});
    cardSprites[randMaquina[4]].setPosition({tamanioVentana.x * .80f, tamanioVentana.y * .15f});
    cardSprites[53].setPosition({tamanioVentana.x * .55f, tamanioVentana.y * .15f});

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
    int cont = 0;
    EstadoJuego estadoActual = MENU;
    sf::Clock relojCartasMaquina;
    int cartasMaquinaMostradas = 0;
    bool mazoMaquina = false;

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
    else if (estadoActual = PARTIDA)
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
              if (mostrarImagen3==true)
              {
                  cardSprites[randJugador[2]].move({tamanioVentana.x * -.05f, 0.f});
                  mostrarImagen4 = true;
              } 
              if (mostrarImagen4 == true && mostrarImagen3 == true){
                cardSprites[randJugador[3]].move({tamanioVentana.x * -.05f, 0.f});
                  mostrarImagen5 = true;
              }
              if (mostrarImagen5 == true && mostrarImagen4 == true && mostrarImagen3 == true){
                cardSprites[randJugador[4]].move({tamanioVentana.x * -.05f, 0.f});
              }

              mostrarImagen3 = true;
              cont ++;
            }
        }        
        else if (accion2 == PLANTARSE)
        {
               relojCartasMaquina.restart(); // Reinicia el tiempo del cronometro creado con Clock
               cartasMaquinaMostradas = 0;
               mazoMaquina = true;

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

        window.draw(cardSprites[randMaquina[0]]);
        window.draw(cardSprites[53]);
        
        if (mazoMaquina) {
            sf::Time tiempo = relojCartasMaquina.getElapsedTime();
        
            if (cartasMaquinaMostradas < 5 && tiempo.asSeconds() > 1.5f * cartasMaquinaMostradas) {
                if (cartasMaquinaMostradas >= 2)
                {
                    cardSprites[randMaquina[0]].move({tamanioVentana.x * -.05f, 0.f});
                    cardSprites[randMaquina[1]].move({tamanioVentana.x * -.05f, 0.f});
                    cardSprites[53].move({tamanioVentana.x * -.05f, 0.f});
                }
                if (cartasMaquinaMostradas >=3)
                {
                   cardSprites[randMaquina[2]].move({tamanioVentana.x * -.05f, 0.f});
                   cardSprites[randMaquina[3]].move({tamanioVentana.x * -.05f, 0.f});
                   cardSprites[randMaquina[4]].move({tamanioVentana.x * -.05f, 0.f});
                }
                cartasMaquinaMostradas++;
            }
        
            for (int i = 0; i < cartasMaquinaMostradas; ++i) {
                window.draw(cardSprites[randMaquina[i]]);
            }
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