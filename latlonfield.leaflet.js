/*jslint browser: true, devel: true, plusplus: true, todo: true, vars: true, white: true */
(function ($, Drupal, L) {
"use strict";

  Drupal.behaviors.LatLonFieldLeaflet = {
    attach: function (context, settings) {

      if (!settings.LatLonField || !settings.LatLonField.Leaflet) {
        return;
      }

      var key;
      for (key in settings.LatLonField.Leaflet) {

        var options = settings.LatLonField.Leaflet[key];
        var mapId   = options.id;
        var items   = options.items;

        // Sorry, but default will be Nantes, FRANCE, if you're unhappy
        // please configure your formatter correctly and set this.
        var center  = new L.LatLng(options.lat || 47.22529200, options.lon || -1.54790800);

        var map = new L.map(mapId, options.map || {});
        map.setView(center, options.zoom || 9);
        // @todo Sorry, hardcoded OSM for now...
        map.addLayer(
          new L.TileLayer(
            'http://{s}.tile.openstreetmap.org/{z}/{x}/{y}.png',
            {
              minZoom: options.minZoom || 8,
              maxZoom: options.maxZoom || 15,
              attribution: options.attribution
            }
          )
        );

//        Drupal.LatLonMaps.maps.push({
//          id: mapId,
//          map: map,
//        });

        /*
        var i;
        for (i = 0; i < items.length; i++) {
          var marker_options = {
            position : new google.maps.LatLng(items[i].latitude, items[i].longitude),
            map : map
          };
          jQuery.extend(marker_options, marker_default_options);
          var marker = new google.maps.Marker(marker_options);
          bounds.extend(marker.getPosition());
        }

        if (items.length > 1) {
          // Reset zoom with bounds if further markers
          map.fitBounds(bounds);
        } else {
          // Otherwise, keep default zoom focus
          map.setCenter(bounds.getCenter());
        }
         */
      }
    }
  };

}(jQuery, Drupal, L));