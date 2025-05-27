if(CONFIG_APDS9306)
    target_sources(app PRIVATE light.c)
endif()

if(CONFIG_BME68X_IAQ)
    target_sources(app PRIVATE environment.c)
endif()

target_sources(app PRIVATE battery.c)