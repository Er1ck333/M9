#include <iostream>
#include <sqlite3.h>
static int callback(void* NotUsed, int argc, char** argv, char** azColName){
    for(int i = 0; i < argc; i++) {
        std::cout << azColName[i] << ": " << (argv[i] ? argv[i] : "NULL") << std::endl;
    }
    return 0;
}

int main() {
    sqlite3* DB;
    char* errorMessage;
    
    // Abrir la conexión a la base de datos (crea una si no existe)
    int exit = sqlite3_open("test.db", &DB);
    
    if (exit) {
        std::cerr << "Error al abrir la base de datos: " << sqlite3_errmsg(DB) << std::endl;
        return exit;
    } else {
        std::cout << "Conexión a la base de datos exitosa!" << std::endl;
    }

    // Crear una tabla si no existe
    std::string createTable = "CREATE TABLE IF NOT EXISTS PERSONAS("
                              "ID INT PRIMARY KEY NOT NULL, "
                              "NOMBRE TEXT NOT NULL, "
                              "EDAD INT NOT NULL);";
    
    exit = sqlite3_exec(DB, createTable.c_str(), NULL, 0, &errorMessage);
    
    if (exit != SQLITE_OK) {
        std::cerr << "Error creando tabla: " << errorMessage << std::endl;
        sqlite3_free(errorMessage);
    } else {
        std::cout << "Tabla creada correctamente!" << std::endl;
    }

    // Insertar valores en la tabla
    std::string insertQuery = "INSERT INTO PERSONAS (ID, NOMBRE, EDAD) VALUES (1, 'Erick', 19);";
    exit = sqlite3_exec(DB, insertQuery.c_str(), NULL, 0, &errorMessage);

    if (exit != SQLITE_OK) {
        std::cerr << "Error al insertar datos: " << errorMessage << std::endl;
        sqlite3_free(errorMessage);
    } else {
        std::cout << "Datos insertados correctamente!" << std::endl;
    }

    // Mostrar los datos de la tabla
    std::string selectQuery = "SELECT * FROM PERSONAS;";
    exit = sqlite3_exec(DB, selectQuery.c_str(), callback, 0, &errorMessage);

    if (exit != SQLITE_OK) {
        std::cerr << "Error al consultar los datos: " << errorMessage << std::endl;
        sqlite3_free(errorMessage);
    }

    // Modificar los datos en la tabla
    std::string updateQuery = "UPDATE PERSONAS SET EDAD = 20 WHERE ID = 1;";
    exit = sqlite3_exec(DB, updateQuery.c_str(), NULL, 0, &errorMessage);

    if (exit != SQLITE_OK) {
        std::cerr << "Error al actualizar los datos: " << errorMessage << std::endl;
        sqlite3_free(errorMessage);
    } else {
        std::cout << "Datos actualizados correctamente!" << std::endl;
    }

    // Volver a mostrar los datos actualizados
    std::cout << "Datos despues de la actualizacion:" << std::endl;
    exit = sqlite3_exec(DB, selectQuery.c_str(), callback, 0, &errorMessage);

    if (exit != SQLITE_OK) {
        std::cerr << "Error al consultar los datos: " << errorMessage << std::endl;
        sqlite3_free(errorMessage);
    }

    // Cerrar la conexión a la base de datos
    sqlite3_close(DB);
    
    return 0;
}
