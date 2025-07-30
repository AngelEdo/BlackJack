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
    INICIAR, 
    SALIR, 
    REINICIAR
};

struct Boton {
    sf::RectangleShape shape;
    bool isOver = false;
    bool isPressedInside = false;
    bool isMousePressed = false;
    BotonAccion tipo = NINGUNA;
};

struct Imagen {
    sf::Texture texture;
    sf::Sprite sprite;
    Imagen() : texture(), sprite(texture) {}
};

struct BotonTexto {
    Boton boton;
    sf::Text texto;
    BotonTexto(const sf::Font& fuente, const std::string& cadena, unsigned int tam)
        : texto(fuente, cadena, tam) {}
};

  //Fnciones del main para la logica 
  BotonAccion manejarBoton(Boton& boton, sf::Vector2f mouse_position, BotonAccion accion);
  bool randRepetido(int rand[], int cantidad, int valor);
  int ajustarAs(int total, int cartas[], int valorCartas[], int cantidad);
  void reiniciarJuego(int randJugador[], int randMaquina[],int valorCartas[], std::vector<sf::Sprite>& sprites,sf::Vector2u tamanioVentana,int& totalJugador, int& totalMaquina);
  Imagen cargarImagen(const std::string& ruta, sf::Vector2f posicion, sf::Vector2f escala);
  void cartasRandom(int* destino, int cantidad, int* otros = nullptr, int otrosTam = 0);
  void posicionarCartas(std::vector<sf::Sprite>& sprites, const int* indices, int cantidad, float y, const std::vector<float>& xMultiplicadores, const sf::Vector2u& tamanioVentana);
  Boton crearBoton(sf::Vector2f size, sf::Vector2f position, BotonAccion tipo);
  BotonTexto crearBotonConTexto(sf::Vector2f size, sf::Vector2f position, BotonAccion tipo, const sf::Font& fuente, const std::string& textoStr, unsigned int tam, sf::Color colorTexto);

 // main del programa
int main()
{
    srand(time(0));
    int randJugador[5];
    int randMaquina[5];
    int valorCartas[53];
    int totalJugador; 
    int totalMaquina;
    // creamos la ventana
    sf::RenderWindow window(sf::VideoMode({900, 600}), "Black jack");

    sf::Vector2u tamanioVentana = window.getSize();
    sf::Font font("C:/Windows/Fonts/arial.ttf");

    //funciones para cargar imagenes 
    Imagen portada = cargarImagen("Cartas/portada.png", {450.f, 300.f}, {1.0f, 1.0f});
    Imagen ganaste = cargarImagen("Cartas/ganaste.png", {450.f, 300.f}, {1.0f, 1.0f});
    Imagen perdiste = cargarImagen("Cartas/perdiste.png", {450.f, 300.f}, {1.0f, 1.0f});
    Imagen empate = cargarImagen("Cartas/empate.png", {450.f, 300.f}, {1.0f, 1.0f});

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
        sprite.setOrigin(bounds.size.componentWiseMul({0.5f, 0.5f}));
        sprite.setScale({0.4f, 0.4f});
        cardSprites.push_back(sprite);
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
    cartasRandom(randJugador, 5);
    cartasRandom(randMaquina, 5, randJugador, 5);
    
    // Cartas volteadas 
    cardSprites[0].setPosition({tamanioVentana.x * .10f, tamanioVentana.y * .50f});
    cardSprites[53].setPosition({tamanioVentana.x * .58f, tamanioVentana.y * .15f});

    // Multiplicadores horizontales para 5 cartas
    std::vector<float> posicionesX = {0.45f, 0.58f, 0.66f, 0.79f, 0.92f};
    // Posicionar cartas del jugador y maquina
    posicionarCartas(cardSprites, randJugador, 5, 0.60f, posicionesX, tamanioVentana);
    posicionarCartas(cardSprites, randMaquina, 5, 0.15f, posicionesX, tamanioVentana);

    //Botones con texto 
    BotonTexto botonIniciar = crearBotonConTexto({210.f, 65.f}, {tamanioVentana.x * .50f, tamanioVentana.y * .89f}, INICIAR, font, "INICIAR", 40, sf::Color::White);
    BotonTexto botonPedir = crearBotonConTexto({200.f, 40.f}, {tamanioVentana.x * .70f, tamanioVentana.y * .90f}, PEDIR_CARTA, font, "PEDIR CARTA", 25, sf::Color::Black);
    BotonTexto botonPlantarse = crearBotonConTexto({180.f, 40.f}, {tamanioVentana.x * .35f, tamanioVentana.y * .90f}, PLANTARSE, font, "PLANTARSE", 25, sf::Color::Black);
    BotonTexto volveraJugar = crearBotonConTexto({250.f, 60.f}, {tamanioVentana.x * .5f, tamanioVentana.y * .90f}, REINICIAR, font,"VOLVER A JUGAR", 25, sf::Color::Black);
    BotonTexto terminarJuego = crearBotonConTexto({120.f, 40.f}, {tamanioVentana.x * .1f, tamanioVentana.y * .05f}, SALIR, font, "SALIR", 25, sf::Color::Black);

    sf::Text llevas(font);
    llevas.setCharacterSize(25);
    llevas.setPosition({tamanioVentana.x * .4f, tamanioVentana.y * .75f});
    llevas.setFillColor(sf::Color::White);

    sf::Text maquinaLleva(font);
    maquinaLleva.setCharacterSize(25);
    maquinaLleva.setPosition({tamanioVentana.x * .35f, tamanioVentana.y * .35f});
    maquinaLleva.setFillColor(sf::Color::White);

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
    sf::Clock relojResultado;
    bool mostrarResultado = false;
    bool botonesHabilitados = true;
    bool botHab = false;
    int tot;
    
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
        BotonAccion accion3 = manejarBoton(botonIniciar.boton, mouse_position, INICIAR);

        if (accion3 == INICIAR)
        {
            estadoActual = PARTIDA;
            totalJugador = valorCartas[randJugador[0]] + valorCartas[randJugador[1]];
            totalMaquina = valorCartas[randMaquina[0]] + valorCartas[randMaquina[1]];
            llevas.setString("Llevas: " + std::to_string(totalJugador) + " Puntos");
            maquinaLleva.setString("La maquina lleva: " + std::to_string(valorCartas[randMaquina[0]]) + " Puntos");
            
        }

        window.clear(sf::Color(64, 64, 64));
        window.draw(portada.sprite);
        window.draw(botonIniciar.boton.shape);
        window.draw(botonIniciar.texto);
        window.display();

    }
    else if (estadoActual == PARTIDA)
    {

        // posicion del mouse 
        auto mouse_position = sf::Vector2f(sf::Mouse::getPosition(window));

         BotonAccion accion1 = NINGUNA;
         BotonAccion accion2 = NINGUNA;
         BotonAccion salir = NINGUNA;
         BotonAccion reiniciar = NINGUNA;

        if (botonesHabilitados) {
            accion1 = manejarBoton(botonPedir.boton, mouse_position, PEDIR_CARTA);
            accion2 = manejarBoton(botonPlantarse.boton, mouse_position, PLANTARSE);
        }
        if (botHab) {
            salir = manejarBoton(terminarJuego.boton, mouse_position, SALIR);
            reiniciar = manejarBoton(volveraJugar.boton, mouse_position, REINICIAR);
        }
        // Manejar botones
        

        if (salir == SALIR) {
            window.close();
        }
        if (reiniciar == REINICIAR){
            estadoActual = PARTIDA;
            mostrarGanaste = mostrarPerdiste = mostrarEmpate = false;
            mostrarImagen3 = mostrarImagen4 = mostrarImagen5 = false;
            mostrarResultado = false;
            botonesHabilitados = true;
            botHab = false;
            mazoMaquina = false;
            cont = 0;
            cartasMaquinaMostradas = 0;
            contMaquinaAnimacion = 2;
            relojCartasMaquina.restart();
            relojResultado.restart();
            
            reiniciarJuego(randJugador, randMaquina, valorCartas, cardSprites, tamanioVentana, totalJugador, totalMaquina);
            llevas.setString("Llevas: " + std::to_string(totalJugador) + " Puntos");
            maquinaLleva.setString("La maquina lleva: " + std::to_string(valorCartas[randMaquina[0]]) + " Puntos");
            
        }
        
        
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
            llevas.setString("Llevas: " + std::to_string(totalJugador) + " Puntos");

            if (totalJugador >= 21) 
            {
               relojCartasMaquina.restart(); 
               cartasMaquinaMostradas = 2;
               contMaquinaAnimacion = 2; 
               mazoMaquina = true;
               tot = valorCartas[randMaquina[0]] + valorCartas[randMaquina[1]];
               totalMaquina = ajustarAs(totalMaquina, randMaquina, valorCartas, cartasMaquinaMostradas);

               //Logica de la maquina para saber quien gana 
                while (totalMaquina < 17 && cartasMaquinaMostradas < 5) {
                    totalMaquina += valorCartas[randMaquina[cartasMaquinaMostradas]];
                    cartasMaquinaMostradas++;
                    totalMaquina = ajustarAs(totalMaquina, randMaquina, valorCartas, cartasMaquinaMostradas);
                }

                maquinaLleva.setString("La maquina lleva: " + std::to_string(tot) + " Puntos");

                // Verificar ganador
                if (totalJugador == 21 && (totalMaquina > 21 || totalMaquina < totalJugador)){
                    mostrarGanaste = true;
                }else if (totalMaquina <= 21){
                    mostrarPerdiste = true;
                }else if (totalMaquina > 21){
                    mostrarEmpate = true;
                }     

              botonesHabilitados = false;
            
            }
            if (cont == 2 && totalJugador <= 21) {
                relojResultado.restart();
                mostrarResultado = true;
                mostrarGanaste = true;
                botonesHabilitados = false;
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
               botonesHabilitados = false;
               tot = valorCartas[randMaquina[0]] + valorCartas[randMaquina[1]];
               totalMaquina = ajustarAs(totalMaquina, randMaquina, valorCartas, cartasMaquinaMostradas);

               //Logica de la maquina para saber quien gana 
                while (totalMaquina < 17 && cartasMaquinaMostradas < 5) {
                    totalMaquina += valorCartas[randMaquina[cartasMaquinaMostradas]];
                    cartasMaquinaMostradas++;
                    totalMaquina = ajustarAs(totalMaquina, randMaquina, valorCartas, cartasMaquinaMostradas);
                }
                            
                maquinaLleva.setString("La maquina lleva: " + std::to_string(tot) + " Puntos");


                // Verificar ganador
                if (totalMaquina > 21 && totalJugador > 21){
                    mostrarEmpate = true;
                }else if (totalMaquina > 21) {
                    mostrarGanaste = true;
                } else if (totalMaquina >= totalJugador) {
                    mostrarPerdiste = true;
                } else if (totalMaquina < totalJugador){     
                    mostrarGanaste = true;
                }           

        }

        window.clear(sf::Color(64, 64, 64));
        if (botonesHabilitados){
          window.draw(botonPedir.boton.shape);
          window.draw(botonPedir.texto);
          window.draw(botonPlantarse.boton.shape);
          window.draw(botonPlantarse.texto);
        }

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
                tot = ajustarAs(0, randMaquina, valorCartas, contMaquinaAnimacion + 1);
                maquinaLleva.setString("La maquina lleva: " + std::to_string(tot) + " Puntos");
               
                contMaquinaAnimacion++;

                if (contMaquinaAnimacion == cartasMaquinaMostradas) {
                    relojResultado.restart();
                    mostrarResultado = true;
                }
                }else if (contMaquinaAnimacion == cartasMaquinaMostradas && !mostrarResultado) {
                  relojResultado.restart();
                  mostrarResultado = true;
                }
        
             for (int i = 0; i < contMaquinaAnimacion; ++i) {
                window.draw(cardSprites[randMaquina[i]]);
            }
        }

        if (mostrarResultado && relojResultado.getElapsedTime().asSeconds() > 5.0f) {
            if (mostrarGanaste){
                window.draw(ganaste.sprite);
            }
            if (mostrarPerdiste){
                window.draw(perdiste.sprite);
            }
            if (mostrarEmpate){
                window.draw(empate.sprite);
            }
            botHab = true;
        }

        if (botHab){
            window.draw(terminarJuego.boton.shape);
            window.draw(terminarJuego.texto);
            window.draw(volveraJugar.boton.shape);
            window.draw(volveraJugar.texto);
          }

        if (estadoActual == PARTIDA && !botHab ){
            window.draw(llevas);
            window.draw(maquinaLleva);
        }
        // display
        window.display();
    
    }
  }

  return 0;
}

Imagen cargarImagen(const std::string& ruta, sf::Vector2f posicion, sf::Vector2f escala) {
    Imagen img;
    if (!img.texture.loadFromFile(ruta)) {
        std::cout << "Error cargando imagen: " << ruta << std::endl;
    }
    img.texture.setSmooth(true);
    img.sprite = sf::Sprite(img.texture);
    sf::FloatRect boundsimagen = img.sprite.getLocalBounds();
    img.sprite.setOrigin(boundsimagen.size.componentWiseMul({0.5f, 0.5f}));
    img.sprite.setPosition(posicion);
    img.sprite.setScale(escala);
    return img;
}

void cartasRandom(int* destino, int cantidad, int* otros, int otrosTam){
    int i = 0;
    while (i < cantidad) {
        int num = 1 + rand() % 52;
        bool repetidoEnDestino = randRepetido(destino, i, num);
        bool repetidoEnOtros = otros && randRepetido(otros, otrosTam, num);

        if (!repetidoEnDestino && !repetidoEnOtros) {
            destino[i] = num;
            i++;
        }
    }
}

void posicionarCartas(std::vector<sf::Sprite>& sprites, const int* indices, int cantidad, float y, const std::vector<float>& xMultiplicadores, const sf::Vector2u& tamanioVentana) {
    for (int i = 0; i < cantidad; ++i) {
        float x = tamanioVentana.x * xMultiplicadores[i];
        float posY = tamanioVentana.y * y;
        sprites[indices[i]].setPosition({x, posY});
    }
}

Boton crearBoton(sf::Vector2f size, sf::Vector2f position, BotonAccion tipo) {
    Boton boton;
    boton.shape.setSize(size);
    boton.shape.setPosition(position);
    boton.shape.setOrigin(size / 2.f);
    boton.tipo = tipo;
    return boton;
}

BotonTexto crearBotonConTexto(sf::Vector2f size, sf::Vector2f position, BotonAccion tipo, const sf::Font& fuente, const std::string& textoStr, unsigned int tam, sf::Color colorTexto) {
    BotonTexto bt(fuente, textoStr, tam);
    bt.boton = crearBoton(size, position, tipo);

    // Crear texto
    bt.texto.setFont(fuente);
    bt.texto.setCharacterSize(tam);
    bt.texto.setString(textoStr);
    sf::FloatRect bounds = bt.texto.getLocalBounds();
    bt.texto.setOrigin({bounds.position.x + bounds.size.x/2, bt.texto.getCharacterSize() * 0.75f});
    bt.texto.setPosition(position); // centrado en el botón
    bt.texto.setFillColor(colorTexto);

    return bt;
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
             boton.shape.setFillColor(sf::Color(0, 171, 255));

         // estado
         boton.isOver = true;
     }
     else
     {
        //cambiamos color a los botones 
        switch (boton.tipo) {
            case REINICIAR:
                boton.shape.setFillColor(sf::Color(118, 252, 165)); 
                break;
            case SALIR:
                boton.shape.setFillColor(sf::Color(204, 92, 92));  
                break;
            case INICIAR:
                boton.shape.setFillColor(sf::Color(232, 150, 122));  
                break;
            default:
               boton.shape.setFillColor(sf::Color::White); 
                break;
        }
         
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

void reiniciarJuego(int randJugador[], int randMaquina[], int valorCartas[], std::vector<sf::Sprite>& sprites, sf::Vector2u tamanioVentana, int& totalJugador, int& totalMaquina)
{
    // Generar nuevas cartas para jugador
    cartasRandom(randJugador, 5);
    cartasRandom(randMaquina, 5, randJugador, 5);

    // Recalcular totales iniciales
    totalJugador = valorCartas[randJugador[0]] + valorCartas[randJugador[1]];
    totalMaquina = valorCartas[randMaquina[0]] + valorCartas[randMaquina[1]];

    // Multiplicadores horizontales para 5 cartas
    std::vector<float> posicionesX = {0.45f, 0.58f, 0.66f, 0.79f, 0.92f};
    // Posicionar cartas del jugador y maquina
    posicionarCartas(sprites, randJugador, 5, 0.60f, posicionesX, tamanioVentana);
    posicionarCartas(sprites, randMaquina, 5, 0.15f, posicionesX, tamanioVentana);
}