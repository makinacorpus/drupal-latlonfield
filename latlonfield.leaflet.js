/*jslint browser: true, devel: true, plusplus: true, todo: true, vars: true, white: true */
(function ($, Drupal /*, L */) {
  "use strict";

  // Avoid double initialization, this would cause JS errors.
  var initialized = {};

  Drupal.behaviors.LatLonFieldLeaflet = {
    attach: function (context, settings) {

      if (!settings.LatLonField || !settings.LatLonField.Leaflet) {
        return;
      }

      if ("undefined" === typeof L) {
        console.log("could not find leaflet, did you include it?");
        return;
      }

      var key;
      for (key in settings.LatLonField.Leaflet) {

        var options = settings.LatLonField.Leaflet[key];
        var mapId   = options.id;

        // Ensure the map exists on the scren
        if (!$('#' + mapId).length) {
          if (console && console.log) {
            console.log("#" + mapId + " container could not be found");
          }
          return;
        }

        if (initialized[options.id]) {
          return; // This map already has been initialized.
        }
        initialized[options.id] = true;

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

        if (options.showBubbles && items) {
          var i, bounds = [];

          for (i = 0; i < items.length; i++) {
            var item = items[i];
            if (!item.latitude || !item.longitude) {
              return;
            }

            var marker = L.marker([item.latitude, item.longitude]);
            marker.addTo(map);
            bounds.push(marker.getLatLng());

            if (item.text) {
              marker.bindPopup(item.text);
            }
          }

          if (bounds.length < 1) {
            map.fitBounds(L.latLngBounds(bounds));
          } else {
            map.panTo(bounds[0]);
          }
        }
      }
    }
  };

}(jQuery, Drupal /*, L */));