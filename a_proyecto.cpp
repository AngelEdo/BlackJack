#include <SFML/Graphics.hpp>
#include <iostream>

enum BotonAccion {
    NINGUNA,
    OCULTAR_IMAGEN,
    MOSTRAR_IMAGEN
};

struct Boton {
    sf::RectangleShape shape;
    bool isOver = false;
    bool isPressedInside = false;
    bool isMousePressed = false;
};

BotonAccion manejarBoton(Boton& boton, sf::Vector2f mouse_position, BotonAccion accion);

// main program
int main()
{
     // create window
     sf::RenderWindow window(sf::VideoMode({800, 600}), "Title");

    // Vector para almacenar las texturas de las cartas
    std::vector<sf::Texture> cardTextures;
    // Vector para almacenar los sprites de las cartas
    std::vector<sf::Sprite> cardSprites;

    // Cargar las imágenes de las cartas (aquí solo estamos cargando 2, pero puedes poner 52)
    for (int i = 0; i < 3; ++i) {
        std::string filename = "Imagenes/carta" + std::to_string(i) + ".png";
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
        sprite.setOrigin(bounds.size.componentWiseMul({0.5f, 0.5f}));  // Centra el sprite
        sprite.setScale({0.15f, 0.15f});  // Cambia el tamaño
        cardSprites.push_back(sprite);  // Guarda el sprite en el vector
    }

    // Posicionar las cartas en la ventana (por ejemplo, las primeras dos cartas)
    sf::Vector2f posicionImagen0(350.f, 400.f);
    sf::Vector2f posicionImagen1(450.f, 400.f);
    sf::Vector2f posicionImagen2(500.f, 400.f);
    cardSprites[0].setPosition(posicionImagen0);
    cardSprites[1].setPosition(posicionImagen1);
    cardSprites[2].setPosition(posicionImagen2);

    // create shape of button
    Boton boton1;
    boton1.shape.setSize({120.f, 40.f});
    boton1.shape.setPosition({500.f, 550.f});
    boton1.shape.setOrigin(boton1.shape.getSize() / 2.f);

    Boton boton2;
    boton2.shape.setSize({120.f, 40.f});
    boton2.shape.setPosition({300.f, 550.f});
    boton2.shape.setOrigin(boton2.shape.getSize() / 2.f);

    bool mostrarImagen = false;

    // window loop
    while (window.isOpen())
    {
        // handle events
        while (auto event = window.pollEvent())
        {
            // on close button release
            if (event->is<sf::Event::Closed>())
            {
                // close window
                window.close();
            }

            // when window is resized
            else if (event->is<sf::Event::Resized>())
            {
                // update view
                window.setView(sf::View(sf::FloatRect({0.f, 0.f}, sf::Vector2f(window.getSize()))));
            }
        }

        // mouse position
        auto mouse_position = sf::Vector2f(sf::Mouse::getPosition(window));

        // Manejar botones
        BotonAccion accion1 = manejarBoton(boton1, mouse_position, OCULTAR_IMAGEN);
        BotonAccion accion2 = manejarBoton(boton2, mouse_position, MOSTRAR_IMAGEN);

        if (accion1 == OCULTAR_IMAGEN)
        {
            posicionImagen0.x -= 50.f;
            cardSprites[0].setPosition(posicionImagen0);
            posicionImagen1.x -= 50.f;
            cardSprites[1].setPosition(posicionImagen1);
            if (mostrarImagen==true)
            {
                posicionImagen2.x -= 50.f;
                cardSprites[2].setPosition(posicionImagen2);
            }
            mostrarImagen = true;

        }        
        else if (accion2 == MOSTRAR_IMAGEN)
            window.close();
        
        // fill window with color
        window.clear(sf::Color(64, 64, 64));
        window.draw(boton1.shape);
        window.draw(boton2.shape);
        
        // Dibujar todas las cartas en el vector
        window.draw(cardSprites[0]);
        window.draw(cardSprites[1]);
        if (mostrarImagen==true)
            {
                window.draw(cardSprites[2]);
            }
            

        // display
        window.display();
    }

    // program end successfully
    return 0;
}

BotonAccion manejarBoton(Boton& boton, sf::Vector2f mouse_position, BotonAccion accion)
{
 
     // state of mouse
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
     
         // on mouse press
         if (onMousePress)
         {
             boton.isPressedInside = true;
         }

         // on mouse release
         if (onMouseRelease and boton.isPressedInside)
         {
             return accion;
         }

         // when mouse is pressed
         if (boton.isMousePressed and boton.isPressedInside)
             boton.shape.setFillColor(sf::Color(0, 170, 255));

         // set state
         boton.isOver = true;
     }
     else
     {
         boton.shape.setFillColor(sf::Color::White);
         // reset state
         boton.isOver = false;
     }
     // reset pressed inside
     if (not boton.isMousePressed)
         boton.isPressedInside = false;

         return NINGUNA;
}