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

BotonAccion manejarBoton(Boton& boton, sf::Vector2f mouse_position, BotonAccion accion);
bool randRepetido(int rand[], int cantidad, int valor);
int ajustarAs(int total, int cartas[], int valorCartas[], int cantidad);
void reiniciarJuego(int randJugador[], int randMaquina[],int valorCartas[], sf::Sprite cardSprites[],sf::Vector2u tamanioVentana,int& totalJugador, int& totalMaquina);

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
    sf::RenderWindow window(sf::VideoMode({900, 600}), "Black jack", sf::Style::Titlebar | sf::Style::Close);
    // sacamos el tamaño de la ventana para acomodar botones imagenes, etc.
    sf::Vector2u tamanioVentana = window.getSize();

    // string
    sf::Font font("C:/Windows/Fonts/arial.ttf");

    sf::Texture portada("Cartas/portada.png");
    portada.setSmooth(true);
    sf::Sprite por(portada);
    sf::FloatRect boundsportada = por.getLocalBounds();
    por.setOrigin(boundsportada.size.componentWiseMul({0.5f, 0.5f}));
    por.setPosition({450.f, 300.f});
    por.setScale({1.0f, 1.0f});

    // Cargamos imagen de ganaste o perdiste 
    sf::Texture ganaste("Cartas/ganaste.png");
    ganaste.setSmooth(true);
    sf::Sprite gan(ganaste);
    sf::FloatRect boundsganaste = gan.getLocalBounds();
    gan.setOrigin(boundsganaste.size.componentWiseMul({0.5f, 0.5f}));
    gan.setPosition({450.f, 300.f});
    gan.setScale({1.0f, 1.0f});

    sf::Texture perdiste("Cartas/perdiste.png");
    perdiste.setSmooth(true);
    sf::Sprite per(perdiste);
    sf::FloatRect boundsperdiste = per.getLocalBounds();
    per.setOrigin(boundsperdiste.size.componentWiseMul({0.5f, 0.5f}));
    per.setPosition({450.f, 300.f});
    per.setScale({1.0f, 1.0f});

    sf::Texture empate("Cartas/empate.png");
    empate.setSmooth(true);
    sf::Sprite emp(empate);
    sf::FloatRect boundsempate = emp.getLocalBounds();
    emp.setOrigin(boundsempate.size.componentWiseMul({0.5f, 0.5f}));
    emp.setPosition({450.f, 300.f});
    emp.setScale({1.0f, 1.0f});

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
    int i = 0;
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
    cardSprites[randJugador[0]].setPosition({tamanioVentana.x * .45f, tamanioVentana.y * .60f});
    cardSprites[randJugador[1]].setPosition({tamanioVentana.x * .58f, tamanioVentana.y * .60f});
    cardSprites[randJugador[2]].setPosition({tamanioVentana.x * .66f, tamanioVentana.y * .60f});
    cardSprites[randJugador[3]].setPosition({tamanioVentana.x * .79f, tamanioVentana.y * .60f});
    cardSprites[randJugador[4]].setPosition({tamanioVentana.x * .92f, tamanioVentana.y * .60f});

    //Cartas aleatorias de la maquina 
    cardSprites[randMaquina[0]].setPosition({tamanioVentana.x * .45f, tamanioVentana.y * .15f});
    cardSprites[randMaquina[1]].setPosition({tamanioVentana.x * .58f, tamanioVentana.y * .15f});
    cardSprites[randMaquina[2]].setPosition({tamanioVentana.x * .66f, tamanioVentana.y * .15f});
    cardSprites[randMaquina[3]].setPosition({tamanioVentana.x * .79f, tamanioVentana.y * .15f});
    cardSprites[randMaquina[4]].setPosition({tamanioVentana.x * .92f, tamanioVentana.y * .15f});
    cardSprites[53].setPosition({tamanioVentana.x * .58f, tamanioVentana.y * .15f});

    // creacion de botones 
    Boton botonPedir;
    botonPedir.shape.setSize({200.f, 40.f});
    botonPedir.shape.setPosition({tamanioVentana.x * .70f, tamanioVentana.y * .90f});
    botonPedir.shape.setOrigin(botonPedir.shape.getSize() / 2.f);
    botonPedir.tipo = PEDIR_CARTA;

    Boton botonPlantarse;
    botonPlantarse.shape.setSize({180.f, 40.f});
    botonPlantarse.shape.setPosition({tamanioVentana.x * .35f, tamanioVentana.y * .90f});
    botonPlantarse.shape.setOrigin(botonPlantarse.shape.getSize() / 2.f);
    botonPlantarse.tipo = PLANTARSE;

    Boton iniciar;
    iniciar.shape.setSize({210.f, 65.f});
    iniciar.shape.setPosition({tamanioVentana.x * .50f, tamanioVentana.y * .89f});
    iniciar.shape.setOrigin(iniciar.shape.getSize() / 2.f);
    iniciar.tipo = INICIAR;

    Boton terminarJuego;
    terminarJuego.shape.setSize({120.f, 40.f});
    terminarJuego.shape.setPosition({tamanioVentana.x * .1f, tamanioVentana.y * .05f});
    terminarJuego.shape.setOrigin(terminarJuego.shape.getSize() / 2.f);
    terminarJuego.tipo = SALIR;

    Boton volveraJugar;
    volveraJugar.shape.setSize({250.f, 60.f});
    volveraJugar.shape.setPosition({tamanioVentana.x * .5f, tamanioVentana.y * .90f});
    volveraJugar.shape.setOrigin(volveraJugar.shape.getSize() / 2.f);
    volveraJugar.tipo = REINICIAR;

    sf::Text in(font);
    in.setCharacterSize(40);
    in.setString("INICIAR");
    sf::FloatRect boundsin = in.getLocalBounds();
    in.setOrigin({boundsin.position.x + boundsin.size.x/2, in.getCharacterSize() * 0.75f});
    in.setPosition(iniciar.shape.getPosition());
    in.setFillColor(sf::Color::White);

    sf::Text pedi(font);
    pedi.setCharacterSize(25);
    pedi.setString("PEDIR CARTA");
    sf::FloatRect boundspedi = pedi.getLocalBounds();
    pedi.setOrigin({boundspedi.position.x + boundspedi.size.x/2, pedi.getCharacterSize() * 0.75f});
    pedi.setPosition(botonPedir.shape.getPosition());
    pedi.setFillColor(sf::Color::Black);

    sf::Text plant(font);
    plant.setCharacterSize(25);
    plant.setString("PLANTARSE");
    sf::FloatRect boundsplant = plant.getLocalBounds();
    plant.setOrigin({boundsplant.position.x + boundsplant.size.x/2, plant.getCharacterSize() * 0.75f});
    plant.setPosition(botonPlantarse.shape.getPosition());
    plant.setFillColor(sf::Color::Black);
    
    sf::Text volver(font);
    volver.setCharacterSize(25);
    volver.setString("VOLVER A JUGAR");
    sf::FloatRect boundsvolver = volver.getLocalBounds();
    volver.setOrigin({boundsvolver.position.x + boundsvolver.size.x/2, volver.getCharacterSize() * 0.75f});
    volver.setPosition(volveraJugar.shape.getPosition());
    volver.setFillColor(sf::Color::Black);

    sf::Text sali(font);
    sali.setCharacterSize(25);
    sali.setString("SALIR");
    sf::FloatRect boundssali = sali.getLocalBounds();
    sali.setOrigin({boundssali.position.x + boundssali.size.x/2, sali.getCharacterSize() * 0.75f});
    sali.setPosition(terminarJuego.shape.getPosition());
    sali.setFillColor(sf::Color::Black);

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
        BotonAccion accion3 = manejarBoton(iniciar, mouse_position, INICIAR);

        if (accion3 == INICIAR)
        {
            estadoActual = PARTIDA;
            totalJugador = valorCartas[randJugador[0]] + valorCartas[randJugador[1]];
            totalMaquina = valorCartas[randMaquina[0]] + valorCartas[randMaquina[1]];
            llevas.setString("Llevas: " + std::to_string(totalJugador) + " Puntos");
            maquinaLleva.setString("La maquina lleva: " + std::to_string(valorCartas[randMaquina[0]]) + " Puntos");
            
        }

        window.clear(sf::Color(64, 64, 64));
        window.draw(por);
        window.draw(iniciar.shape);
        window.draw(in);
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
            accion1 = manejarBoton(botonPedir, mouse_position, PEDIR_CARTA);
            accion2 = manejarBoton(botonPlantarse, mouse_position, PLANTARSE);
        }
        if (botHab) {
            salir = manejarBoton(terminarJuego, mouse_position, SALIR);
            reiniciar = manejarBoton(volveraJugar, mouse_position, REINICIAR);
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
            
            reiniciarJuego(randJugador, randMaquina, valorCartas, cardSprites.data(), tamanioVentana, totalJugador, totalMaquina);
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
          window.draw(botonPedir.shape);
          window.draw(pedi);
          window.draw(botonPlantarse.shape);
          window.draw(plant);
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
                window.draw(gan);
            }
            if (mostrarPerdiste){
                window.draw(per);
            }
            if (mostrarEmpate){
                window.draw(emp);
            }
            botHab = true;
        }

        if (botHab){
            window.draw(terminarJuego.shape);
            window.draw(sali);
            window.draw(volveraJugar.shape);
            window.draw(volver);
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

void reiniciarJuego(int randJugador[], int randMaquina[],int valorCartas[], sf::Sprite cardSprites[],sf::Vector2u tamanioVentana,int& totalJugador, int& totalMaquina)
{
    // Generar nuevas cartas para jugador
    int i = 0;
    while (i < 5) {
        int num = 1 + rand() % 52;
        if (!randRepetido(randJugador, i, num)) {
            randJugador[i] = num;
            i++;
        }
    }

    // Generar nuevas cartas para maquina
    i = 0;
    while (i < 5) {
        int num = 1 + rand() % 52;
        if (!randRepetido(randJugador, 5, num) && !randRepetido(randMaquina, i, num)) {
            randMaquina[i] = num;
            i++;
        }
    }

    // Recalcular totales iniciales
    totalJugador = valorCartas[randJugador[0]] + valorCartas[randJugador[1]];
    totalMaquina = valorCartas[randMaquina[0]] + valorCartas[randMaquina[1]];

    // Reposicionar cartas del jugador
    cardSprites[randJugador[0]].setPosition({tamanioVentana.x * .45f, tamanioVentana.y * .60f});
    cardSprites[randJugador[1]].setPosition({tamanioVentana.x * .58f, tamanioVentana.y * .60f});
    cardSprites[randJugador[2]].setPosition({tamanioVentana.x * .66f, tamanioVentana.y * .60f});
    cardSprites[randJugador[3]].setPosition({tamanioVentana.x * .79f, tamanioVentana.y * .60f});
    cardSprites[randJugador[4]].setPosition({tamanioVentana.x * .92f, tamanioVentana.y * .60f});

    // Reposicionar cartas de la maquina
    cardSprites[randMaquina[0]].setPosition({tamanioVentana.x * .45f, tamanioVentana.y * .15f});
    cardSprites[randMaquina[1]].setPosition({tamanioVentana.x * .58f, tamanioVentana.y * .15f});
    cardSprites[randMaquina[2]].setPosition({tamanioVentana.x * .66f, tamanioVentana.y * .15f});
    cardSprites[randMaquina[3]].setPosition({tamanioVentana.x * .79f, tamanioVentana.y * .15f});
    cardSprites[randMaquina[4]].setPosition({tamanioVentana.x * .92f, tamanioVentana.y * .15f});
}