#include <vtkAutoInit.h>
VTK_MODULE_INIT(vtkRenderingOpenGL2);
VTK_MODULE_INIT(vtkInteractionStyle);

#include <vtkSmartPointer.h>
#include <vtkCubeSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkCommand.h>
#include <vtkCallbackCommand.h>
#include <iostream>
#include <string>

// Estructura para mantener el estado de la rotación
struct AnimationState {
    vtkSmartPointer<vtkActor> actor;
    vtkSmartPointer<vtkRenderWindow> renderWindow;
    char axis = ' '; // 'x', 'y', 'z' o ' ' (detenido)
    double speed = 1.0; // Grados por frame
};

// Callback que se ejecuta en cada frame (Timer Event)
void TimerCallbackFunction(vtkObject* caller, unsigned long vtkNotUsed(eventId), 
                           void* clientData, void* vtkNotUsed(callData)) {
    AnimationState* state = static_cast<AnimationState*>(clientData);
    
    if (state->axis == 'x') {
        state->actor->RotateX(state->speed);
    } else if (state->axis == 'y') {
        state->actor->RotateY(state->speed);
    } else if (state->axis == 'z') {
        state->actor->RotateZ(state->speed);
    } else {
        return; // Si es 'space' o vacío, no redibujamos innecesariamente
    }

    // Solicita renderizar el frame actualizado
    state->renderWindow->Render();
}

// Callback que escucha las teclas del teclado
void KeypressCallbackFunction(vtkObject* caller, unsigned long vtkNotUsed(eventId), 
                              void* clientData, void* vtkNotUsed(callData)) {
    vtkRenderWindowInteractor* interactor = static_cast<vtkRenderWindowInteractor*>(caller);
    AnimationState* state = static_cast<AnimationState*>(clientData);
    
    std::string key = interactor->GetKeySym();

    if (key == "x" || key == "X") {
        state->axis = 'x';
        std::cout << "[Teclado] Rotando sobre eje X" << std::endl;
    } else if (key == "y" || key == "Y") {
        state->axis = 'y';
        std::cout << "[Teclado] Rotando sobre eje Y" << std::endl;
    } else if (key == "z" || key == "Z") {
        state->axis = 'z';
        std::cout << "[Teclado] Rotando sobre eje Z" << std::endl;
    } else if (key == "space") {
        state->axis = ' ';
        std::cout << "[Teclado] Rotación detenida" << std::endl;
    }
}

// NUEVO: Callback que escucha los clics del mouse
void MouseClickCallbackFunction(vtkObject* caller, unsigned long eventId, 
                                 void* vtkNotUsed(clientData), void* vtkNotUsed(callData)) {
    vtkRenderWindowInteractor* interactor = static_cast<vtkRenderWindowInteractor*>(caller);
    
    // Obtener la posición del cursor en píxeles (X, Y) relativos a la ventana
    int clickPos[2];
    interactor->GetEventPosition(clickPos);

    if (eventId == vtkCommand::LeftButtonPressEvent) {
        std::cout << "[Mouse] Click Izquierdo PRESIONADO en la posición: (" 
                  << clickPos[0] << ", " << clickPos[1] << ")" << std::endl;
    } else if (eventId == vtkCommand::LeftButtonReleaseEvent) {
        std::cout << "[Mouse] Click Izquierdo SOLTADO en la posición: (" 
                  << clickPos[0] << ", " << clickPos[1] << ")" << std::endl;
    }
}

int main() {
    // 1. Crear la geometría del cubo
    auto cubeSource = vtkSmartPointer<vtkCubeSource>::New();
    cubeSource->SetXLength(2.0);
    cubeSource->SetYLength(2.0);
    cubeSource->SetZLength(2.0);
    cubeSource->Update();

    // 2. Configurar el Mapper y Actor
    auto mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    mapper->SetInputConnection(cubeSource->GetOutputPort());

    auto actor = vtkSmartPointer<vtkActor>::New();
    actor->SetMapper(mapper);

    // 3. Configurar el entorno de Renderizado (Renderer y Window)
    auto renderer = vtkSmartPointer<vtkRenderer>::New();
    renderer->AddActor(actor);
    renderer->SetBackground(0.1, 0.2, 0.3); // Fondo azul oscuro/gris

    auto renderWindow = vtkSmartPointer<vtkRenderWindow>::New();
    renderWindow->AddRenderer(renderer);
    renderWindow->SetWindowName("VTK Demo - Cubo Interactivo");
    renderWindow->SetSize(800, 600);

    // 4. Configurar el Interactor y Estilo (Rotación y Zoom nativos por defecto)
    auto interactor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
    interactor->SetRenderWindow(renderWindow);

    auto style = vtkSmartPointer<vtkInteractorStyleTrackballCamera>::New();
    interactor->SetInteractorStyle(style);

    // Estado compartido para las animaciones
    AnimationState animState;
    animState.actor = actor;
    animState.renderWindow = renderWindow;

    // 5. Registrar Callbacks para eventos de teclado, timer y MOUSE
    auto keyCallback = vtkSmartPointer<vtkCallbackCommand>::New();
    keyCallback->SetCallback(KeypressCallbackFunction);
    keyCallback->SetClientData(&animState);
    interactor->AddObserver(vtkCommand::KeyPressEvent, keyCallback);

    auto timerCallback = vtkSmartPointer<vtkCallbackCommand>::New();
    timerCallback->SetCallback(TimerCallbackFunction);
    timerCallback->SetClientData(&animState);
    interactor->AddObserver(vtkCommand::TimerEvent, timerCallback);

    // NUEVO: Registrar el callback para capturar clicks del mouse
    auto mouseCallback = vtkSmartPointer<vtkCallbackCommand>::New();
    mouseCallback->SetCallback(MouseClickCallbackFunction);
    // Añadimos observadores tanto para cuando presionas como para cuando sueltas el click
    interactor->AddObserver(vtkCommand::LeftButtonPressEvent, mouseCallback);
    interactor->AddObserver(vtkCommand::LeftButtonReleaseEvent, mouseCallback);

    // 6. Inicializar y arrancar el bucle de eventos
    renderWindow->Render();
    interactor->Initialize();
    
    // Crear un timer continuo (cada 10 milisegundos aprox.) para la animación fluida
    interactor->CreateRepeatingTimer(10); 

    std::cout << "Controles:\n"
              << " - Arrastrar Mouse izquierdo: Rotar cámara (verás logs de clicks en consola)\n"
              << " - Scroll / Mouse derecho: Zoom\n"
              << " - Presionar 'x', 'y', 'z': Activar rotación continua del cubo\n"
              << " - Presionar 'Espacio': Detener rotación" << std::endl;

    interactor->Start();

    return 0;
}