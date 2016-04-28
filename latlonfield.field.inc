<?php

/**
 * @file
 * Implement a latlon field.
 */

/**
 * Implements hook_field_info().
 */
function latlonfield_field_info() {
  return [
    'latlon' => [
      'label'             => t('Lat/Lon'),
      'description'       => t('Stores latitude/longitude coordinates'),
      'settings'          => [],
      'instance_settings' => [],
      'default_widget'    => 'latlon',
      'default_formatter' => 'latlon',
    ],
    'address' => [
      'label'             => t('Address'),
      'description'       => t('Stores an address'),
      'settings'          => [],
      'instance_settings' => ['default_country' => 'fr'],
      'default_widget'    => 'address_fr_afnor',
      'default_formatter' => 'address_fr_afnor',
    ],
  ];
}

/**
 * Implements hook_field_is_empty().
 */
function latlonfield_field_is_empty($item, $field) {
  switch ($field['type']) {

    case 'latlon':
      return empty($item['latitude']) || empty($item['longitude']);

    case 'address':
      return empty($item['name_line']) || empty($item['locality']) || empty($item['country']);
  }
}
