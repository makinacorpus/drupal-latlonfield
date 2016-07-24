/*jslint browser: true, devel: true, plusplus: true, todo: true, vars: true, white: true */
(function ($, Drupal, google) {
"use strict";

Drupal.LatLonMaps = {

  maps: [],

  resize: function (map_id) {
    var i, current;
    for (i = 0; i < Drupal.LatLonMaps.maps.length; ++i) {
      current = Drupal.LatLonMaps.maps[i];
      if (!map_id || current.id === map_id) {
        google.maps.event.trigger(current.map, 'resize');
        current.map.setCenter(current.bounds.getCenter());
      }
    }
  }
};

Drupal.behaviors.LatLonFieldGmap = {
  attach: function (context, settings) {

    var map_id = settings.LatLonField.Gmap.id;
    var map_default_options = settings.LatLonField.Gmap.map_options;
    var marker_default_options = settings.LatLonField.Gmap.marker_options;
    var items = settings.LatLonField.Gmap.items;

    $('#' + map_id, context).once('map', function() {

      var map_options = {
        center : new google.maps.LatLng(0, 0),
        zoom: 15,
        mapTypeControl : false,
        streetViewControl : false
      };
      jQuery.extend(map_options, map_default_options);

      var i;
      var map = new google.maps.Map(this, map_options);
      var bounds = new google.maps.LatLngBounds();

      // Register map for external usage
      Drupal.LatLonMaps.maps.push({
        id: map_id,
        map: map,
        bounds: bounds
      });

      for (i = 0; i < items.length; i++) {

        var marker_options = {
          position : new google.maps.LatLng(items[i].latitude, items[i].longitude),
          map : map
        };
        jQuery.extend(marker_options, marker_default_options);

        var marker = new google.maps.Marker(marker_options);
        bounds.extend(marker.getPosition());
      }

      // Reset zoom with bounds if further markers
      if (items.length > 1) {
        map.fitBounds(bounds);
      }
      //Otherwise, keep default zoom focus
      else {
        map.setCenter(bounds.getCenter());
      }
    });
  }
};

}(jQuery, Drupal, google));