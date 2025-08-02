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

struct Jugador {
    int rand[5];
    int total = 0;
    int cont = 0;
    bool mostrarCarta[3] = {false};

};

struct Maquina {
    int rand[5];                     
    int totalMaquina = 0;         
    int tot = 0;          
    bool mazoVisible = false;      
    int cartasMostradas = 0; 
    int contAnimacion = 2;        
    sf::Clock relojCartas;       
};

struct JuegoEstado {
    EstadoJuego estadoActual = MENU;
    bool mostrarGanaste = false;
    bool mostrarPerdiste = false;
    bool mostrarEmpate = false;
    bool mostrarResultado = false;
    bool botonesHabilitados = true;
    bool botonesFinalesHabilitados = false;
    sf::Clock relojResultado;
};

  //Fnciones del main para la logica 
  BotonAccion manejarBoton(Boton& boton, sf::Vector2f mouse_position, BotonAccion accion);
  bool randRepetido(int rand[], int cantidad, int valor);
  int ajustarAs(int total, int cartas[], int valorCartas[], int cantidad);
  void reiniciarJuego(Jugador &jug, Maquina &maqui, int valorCartas[], std::vector<sf::Sprite>& sprites, sf::Vector2u tamanioVentana);
  Imagen cargarImagen(const std::string& ruta, sf::Vector2f posicion, sf::Vector2f escala);
  void cartasRandom(int* destino, int cantidad, int* otros = nullptr, int otrosTam = 0);
  void posicionarCartas(std::vector<sf::Sprite>& sprites, const int* indices, int cantidad, float y, const std::vector<float>& xMultiplicadores, const sf::Vector2u& tamanioVentana);
  Boton crearBoton(sf::Vector2f size, sf::Vector2f position, BotonAccion tipo);
  BotonTexto crearBotonConTexto(sf::Vector2f size, sf::Vector2f position, BotonAccion tipo, const sf::Font& fuente, const std::string& textoStr, unsigned int tam, sf::Color colorTexto);

 // main del programa
int main()
{
    srand(time(0));
    int valorCartas[53];
    Jugador jug;
    Maquina maqui;
    JuegoEstado juego;

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
    cartasRandom(jug.rand, 5);
    cartasRandom(maqui.rand, 5, jug.rand, 5);
    
    // Cartas volteadas 
    cardSprites[0].setPosition({tamanioVentana.x * .10f, tamanioVentana.y * .50f});
    cardSprites[53].setPosition({tamanioVentana.x * .58f, tamanioVentana.y * .15f});

    // Multiplicadores horizontales para 5 cartas
    std::vector<float> posicionesX = {0.45f, 0.58f, 0.66f, 0.79f, 0.92f};
    // Posicionar cartas del jugador y maquina
    posicionarCartas(cardSprites, jug.rand, 5, 0.60f, posicionesX, tamanioVentana);
    posicionarCartas(cardSprites, maqui.rand, 5, 0.15f, posicionesX, tamanioVentana);

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
    if (juego.estadoActual == MENU)
    {
        auto mouse_position = sf::Vector2f(sf::Mouse::getPosition(window));
        BotonAccion accion3 = manejarBoton(botonIniciar.boton, mouse_position, INICIAR);

        if (accion3 == INICIAR)
        {
            juego.estadoActual = PARTIDA;
            jug.total = valorCartas[jug.rand[0]] + valorCartas[jug.rand[1]];
            maqui.totalMaquina = valorCartas[maqui.rand[0]] + valorCartas[maqui.rand[1]];
            llevas.setString("Llevas: " + std::to_string(jug.total) + " Puntos");
            maquinaLleva.setString("La maquina lleva: " + std::to_string(valorCartas[maqui.rand[0]]) + " Puntos");
            
        }

        window.clear(sf::Color(64, 64, 64));
        window.draw(portada.sprite);
        window.draw(botonIniciar.boton.shape);
        window.draw(botonIniciar.texto);
        window.display();

    }
    else if (juego.estadoActual == PARTIDA)
    {

        // posicion del mouse 
        auto mouse_position = sf::Vector2f(sf::Mouse::getPosition(window));

         BotonAccion accion1 = NINGUNA;
         BotonAccion accion2 = NINGUNA;
         BotonAccion salir = NINGUNA;
         BotonAccion reiniciar = NINGUNA;

        if (juego.botonesHabilitados) {
            accion1 = manejarBoton(botonPedir.boton, mouse_position, PEDIR_CARTA);
            accion2 = manejarBoton(botonPlantarse.boton, mouse_position, PLANTARSE);
        }
        if (juego.botonesFinalesHabilitados) {
            salir = manejarBoton(terminarJuego.boton, mouse_position, SALIR);
            reiniciar = manejarBoton(volveraJugar.boton, mouse_position, REINICIAR);
        }
        // Manejar botones
        

        if (salir == SALIR) {
            window.close();
        }
        if (reiniciar == REINICIAR){
            juego.estadoActual = PARTIDA;
            juego.mostrarGanaste = juego.mostrarPerdiste = juego.mostrarEmpate = false;
            jug.mostrarCarta[0] = jug.mostrarCarta[1] = jug.mostrarCarta[2] = false;
            juego.mostrarResultado = false;
            juego.botonesHabilitados = true;
            juego.botonesFinalesHabilitados = false;
            maqui.mazoVisible = false;
            jug.cont = 0;
            maqui.cartasMostradas = 0;
            maqui.contAnimacion = 2;
            maqui.relojCartas.restart();
            juego.relojResultado.restart();
            
            reiniciarJuego(jug, maqui, valorCartas, cardSprites, tamanioVentana);
            llevas.setString("Llevas: " + std::to_string(jug.total) + " Puntos");
            maquinaLleva.setString("La maquina lleva: " + std::to_string(valorCartas[maqui.rand[0]]) + " Puntos");
            
        }
        
        //Condicional del jugador 
        if (accion1 == PEDIR_CARTA)
        {
            if (jug.cont < 3)
            {
                cardSprites[jug.rand[0]].move({tamanioVentana.x * -.05f, 0.f});
                cardSprites[jug.rand[1]].move({tamanioVentana.x * -.05f, 0.f});
              if (jug.mostrarCarta[0] == true )
              {
                  cardSprites[jug.rand[2]].move({tamanioVentana.x * -.05f, 0.f});
                  jug.mostrarCarta[1] = true;
              } 
              if (jug.mostrarCarta[1] == true ){
                  cardSprites[jug.rand[3]].move({tamanioVentana.x * -.05f, 0.f});
                  jug.mostrarCarta[2] = true;
              }
              if (jug.mostrarCarta[2] == true ){
                 cardSprites[jug.rand[4]].move({tamanioVentana.x * -.05f, 0.f});
              }

              jug.mostrarCarta[0] = true;
             
            // Aqui desarrolamos la logica del juego para saber si el jugador gano o perdio
            int cartasEnJuego = 2 + jug.cont + 1;
            jug.total = ajustarAs(0, jug.rand, valorCartas, cartasEnJuego);
            llevas.setString("Llevas: " + std::to_string(jug.total) + " Puntos");

            if (jug.total >= 21) 
            {
               maqui.relojCartas.restart(); 
               maqui.cartasMostradas = 2;
               maqui.contAnimacion = 2; 
               maqui.mazoVisible = true;
               maqui.tot = valorCartas[maqui.rand[0]] + valorCartas[maqui.rand[1]];
               maqui.totalMaquina = ajustarAs(maqui.totalMaquina, maqui.rand, valorCartas, maqui.cartasMostradas);

               //Logica de la maquina para saber quien gana 
                while (maqui.totalMaquina < 17 && maqui.cartasMostradas < 5) {
                    maqui.totalMaquina += valorCartas[maqui.rand[maqui.cartasMostradas]];
                    maqui.cartasMostradas++;
                    maqui.totalMaquina = ajustarAs(maqui.totalMaquina, maqui.rand, valorCartas, maqui.cartasMostradas);
                }

                maquinaLleva.setString("La maquina lleva: " + std::to_string(maqui.tot) + " Puntos");

                // Verificar ganador
                if (jug.total == 21 && (maqui.totalMaquina > 21 || maqui.totalMaquina < jug.total)){
                    juego.mostrarGanaste = true;
                }else if (maqui.totalMaquina <= 21){
                    juego.mostrarPerdiste = true;
                }else if (maqui.totalMaquina > 21){
                    juego.mostrarEmpate = true;
                }     

              juego.botonesHabilitados = false;
            
            }
            if (jug.cont == 2 && jug.total <= 21) {
                juego.relojResultado.restart();
                juego.mostrarResultado = true;
                juego.mostrarGanaste = true;
                juego.botonesHabilitados = false;
            }

              jug.cont ++;
            }


        }        
        else if (accion2 == PLANTARSE)
        {
               maqui.relojCartas.restart(); 
               maqui.cartasMostradas = 2;
               maqui.contAnimacion = 2; 
               maqui.mazoVisible = true;
               juego.botonesHabilitados = false;
               maqui.tot = valorCartas[maqui.rand[0]] + valorCartas[maqui.rand[1]];
               maqui.totalMaquina = ajustarAs(maqui.totalMaquina, maqui.rand, valorCartas, maqui.cartasMostradas);

               //Logica de la maquina para saber quien gana 
                while (maqui.totalMaquina < 17 && maqui.cartasMostradas < 5) {
                    maqui.totalMaquina += valorCartas[maqui.rand[maqui.cartasMostradas]];
                    maqui.cartasMostradas++;
                    maqui.totalMaquina = ajustarAs(maqui.totalMaquina, maqui.rand, valorCartas, maqui.cartasMostradas);
                }
                            
                maquinaLleva.setString("La maquina lleva: " + std::to_string(maqui.tot) + " Puntos");


                // Verificar ganador
                if (maqui.totalMaquina > 21 && jug.total > 21){
                    juego.mostrarEmpate = true;
                }else if (maqui.totalMaquina > 21) {
                    juego.mostrarGanaste = true;
                } else if (maqui.totalMaquina >= jug.total) {
                    juego.mostrarPerdiste = true;
                } else if (maqui.totalMaquina < jug.total){     
                    juego.mostrarGanaste = true;
                }           

        }

        window.clear(sf::Color(64, 64, 64));
        if (juego.botonesHabilitados){
          window.draw(botonPedir.boton.shape);
          window.draw(botonPedir.texto);
          window.draw(botonPlantarse.boton.shape);
          window.draw(botonPlantarse.texto);
        }

        // Llamamos a las cartas para ser dibujadas 
        window.draw(cardSprites[0]);
        window.draw(cardSprites[jug.rand[0]]);
        window.draw(cardSprites[jug.rand[1]]);
        if (jug.mostrarCarta[0]){
            window.draw(cardSprites[jug.rand[2]]);
        }
        if (jug.mostrarCarta[1]){
            window.draw(cardSprites[jug.rand[3]]);
        }
        if (jug.cont == 3){
            window.draw(cardSprites[jug.rand[4]]);
        }

        if (!maqui.mazoVisible) {
            window.draw(cardSprites[maqui.rand[0]]);
            window.draw(cardSprites[53]);
        }
        
        //logica del juego de la maquina
        if (maqui.mazoVisible) {
            sf::Time tiempo = maqui.relojCartas.getElapsedTime();

            if (maqui.contAnimacion < maqui.cartasMostradas && tiempo.asSeconds() > 1.5f * maqui.contAnimacion) {
                int index = maqui.contAnimacion;
                if (index == 2){
                    cardSprites[maqui.rand[0]].move({tamanioVentana.x * -0.05f, 0.f});
                    cardSprites[maqui.rand[1]].move({tamanioVentana.x * -0.05f, 0.f});
                }
                if (index > 2) {
                    cardSprites[maqui.rand[0]].move({tamanioVentana.x * -0.05f, 0.f});
                    cardSprites[maqui.rand[1]].move({tamanioVentana.x * -0.05f, 0.f});
                    cardSprites[maqui.rand[2]].move({tamanioVentana.x * -0.05f, 0.f});
                    cardSprites[maqui.rand[3]].move({tamanioVentana.x * -0.05f, 0.f});
                    cardSprites[maqui.rand[4]].move({tamanioVentana.x * -0.05f, 0.f});
                }
                maqui.tot = ajustarAs(0, maqui.rand, valorCartas, maqui.contAnimacion + 1);
                maquinaLleva.setString("La maquina lleva: " + std::to_string(maqui.tot) + " Puntos");
               
                maqui.contAnimacion++;

                if (maqui.contAnimacion == maqui.cartasMostradas) {
                    juego.relojResultado.restart();
                    juego.mostrarResultado = true;
                }
                }else if (maqui.contAnimacion == maqui.cartasMostradas && !juego.mostrarResultado) {
                  juego.relojResultado.restart();
                  juego.mostrarResultado = true;
                }
        
             for (int i = 0; i < maqui.contAnimacion; ++i) {
                window.draw(cardSprites[maqui.rand[i]]);
            }
        }

        if (juego.mostrarResultado && juego.relojResultado.getElapsedTime().asSeconds() > 5.0f) {
            if (juego.mostrarGanaste){
                window.draw(ganaste.sprite);
            }
            if (juego.mostrarPerdiste){
                window.draw(perdiste.sprite);
            }
            if (juego.mostrarEmpate){
                window.draw(empate.sprite);
            }
            juego.botonesFinalesHabilitados = true;
        }

        if (juego.botonesFinalesHabilitados){
            window.draw(terminarJuego.boton.shape);
            window.draw(terminarJuego.texto);
            window.draw(volveraJugar.boton.shape);
            window.draw(volveraJugar.texto);
          }

        if (juego.estadoActual == PARTIDA && !juego.botonesFinalesHabilitados ){
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

void reiniciarJuego(Jugador &jug, Maquina &maqui, int valorCartas[], std::vector<sf::Sprite>& sprites, sf::Vector2u tamanioVentana)
{
    // Generar nuevas cartas para jugador
    cartasRandom(jug.rand, 5);
    cartasRandom(maqui.rand, 5, jug.rand, 5);

    // Recalcular totales iniciales
    jug.total = valorCartas[jug.rand[0]] + valorCartas[jug.rand[1]];
    maqui.totalMaquina = valorCartas[maqui.rand[0]] + valorCartas[maqui.rand[1]];

    // Multiplicadores horizontales para 5 cartas
    std::vector<float> posicionesX = {0.45f, 0.58f, 0.66f, 0.79f, 0.92f};
    // Posicionar cartas del jugador y maquina
    posicionarCartas(sprites, jug.rand, 5, 0.60f, posicionesX, tamanioVentana);
    posicionarCartas(sprites, maqui.rand, 5, 0.15f, posicionesX, tamanioVentana);
}