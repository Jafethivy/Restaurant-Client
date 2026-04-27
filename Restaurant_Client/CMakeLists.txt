cmake_minimum_required(VERSION 3.22)
project(Restaurant_Client LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(PROJECT_NAME "Restaurant_Client")

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_AUTOUIC_SEARCH_PATHS 
    ${CMAKE_CURRENT_SOURCE_DIR}/view
    ${CMAKE_CURRENT_SOURCE_DIR}/view/qt/login
    ${CMAKE_CURRENT_SOURCE_DIR}/view/qt/utils
    ${CMAKE_CURRENT_SOURCE_DIR}/view/qt/reception
)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR}
    COMPONENTS
        Core
        Gui
        Widgets
        Quick
        QuickWidgets
        Qml
        Network
        HttpServer
)
qt_standard_project_setup()

set(QML_VIEW
    ${CMAKE_CURRENT_SOURCE_DIR}/view/qml
)
set(QT_VIEW
    ${CMAKE_CURRENT_SOURCE_DIR}/view/qt
)
set(VIEW
    ${CMAKE_CURRENT_SOURCE_DIR}/view/MainWindow.ui
    ${CMAKE_CURRENT_SOURCE_DIR}/view/qt/utils/error.ui

    ${QT_VIEW}/login/Login.ui
    ${QT_VIEW}/reception/Reception.ui
)

set(LOGIN
    ${CMAKE_CURRENT_SOURCE_DIR}/src/login
)
set(RECEPTION
    ${CMAKE_CURRENT_SOURCE_DIR}/src/reception
)
set(SRC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/MainWindow.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/MainWindow.cpp

    ${LOGIN}/Login.h
    ${LOGIN}/Login.cpp
    ${LOGIN}/LoginController.h
    ${LOGIN}/LoginController.cpp

    ${RECEPTION}/Reception.h
    ${RECEPTION}/Reception.cpp
    ${RECEPTION}/ReceptionController.h
    ${RECEPTION}/ReceptionController.cpp
)

set(AUTH_SERVICE
    ${CMAKE_CURRENT_SOURCE_DIR}/services/authService
)
set(RECEPTION_SERVICE
    ${CMAKE_CURRENT_SOURCE_DIR}/services/receptionService
)
set(SERVICES
    ${AUTH_SERVICE}/AuthService.h
    ${AUTH_SERVICE}/AuthService.cpp
    ${RECEPTION_SERVICE}/ReservationService.h
    ${RECEPTION_SERVICE}/ReservationService.cpp
)

set(PROJECT_SOURCES
    main.cpp
    AppContext.h
    AppContext.cpp
    ${VIEW}
    ${SRC}
    ${SERVICES}
)
qt_add_executable(${PROJECT_NAME} ${PROJECT_SOURCES})


#QML Configuration
qt_policy(SET QTP0004 NEW)
qt_policy(SET QTP0001 NEW)

qt_add_qml_module(${PROJECT_NAME}
    URI ${PROJECT_NAME}
    VERSION 1.0
    
    QML_FILES
        view/qml/reception/Reservations.qml
        view/qml/reception/Reception.qml
        view/qml/reception/Config.qml
        view/qml/reception/Tables.qml
        view/qml/reception/FilterPanel.qml

        view/qml/reception/filterComponents/SectionLabel.qml
        view/qml/reception/filterComponents/ToggleFilterButton.qml
        view/qml/reception/filterComponents/TimeRangeField.qml
        view/qml/reception/filterComponents/DateRangeField.qml
        view/qml/reception/filterComponents/StyledSpinBox.qml
        view/qml/reception/filterComponents/MiniToggle.qml
        
        view/qml/reception/generalComponents/CloseButton.qml
)

set_target_properties(${PROJECT_NAME}
    PROPERTIES
        WIN32_EXECUTABLE TRUE
)

target_link_libraries(${PROJECT_NAME}
    PUBLIC
    Qt::Core
    Qt::Gui
    Qt::Widgets
    Qt::Quick
    Qt::QuickWidgets
    Qt::Qml
    Qt::Network
    Qt::HttpServer
)