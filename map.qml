import QtQuick 2.0
import QtLocation 5.6
import QtPositioning 5.6

Rectangle {
    visible: true

    Plugin {
        id: mapPlugin
        name: "osm" // "mapboxgl", "esri", ...
        // specify plugin parameters if necessary
        //PluginParameter {
            //name: 'osm.mapping.offline.directory'
            //value: ':/Tiles/'
        //}
    }

    Map {
        anchors.fill: parent
        plugin: mapPlugin
        center: QtPositioning.coordinate(53.59, 19.55) // Oslo
        zoomLevel: 14
    }
}
