<?php

/**
 * Implements hook_field_formatter_info().
 */
function latlonfield_field_formatter_info() {
  return [
    'latlonfield' => [
      'label'       => t('Default'),
      'field types' => ['latlon'],
      'settings'    => [],
    ],
    'latlonfield_gmap' => [
      'label'       => t('Google map'),
      'field types' => ['latlon'],
      'settings'    => [],
    ],
    'latlonfield_leaflet' => [
      'label'       => t('Leaflet'),
      'field types' => ['latlon'],
      'settings'    => [],
    ],
    'address' => [
      'label'       => t('Default'),
      'field types' => ['address'],
      'settings'    => [
        'fields' => [],
      ],
    ],
    'address_fr_afnor' => [
      'label'       => t('French SNA/AFNOR standard'),
      'field types' => ['address'],
      'settings'    => [],
    ],
  ];
}

/**
 * Implements hook_field_formatter_settings_form().
 */
function latlonfield_field_formatter_settings_form($field, $instance, $view_mode, $form, &$form_state) {
  $element = [];
  $display = $instance['display'][$view_mode];
  $settings = $display['settings'];

  if ($display['type'] == 'address') {
    $element['fields'] = [
      '#title'         => t('Fields to display'),
      '#type'          => 'checkboxes',
      '#options'       => drupal_map_assoc([
        '',
        'name_line',
        'organisation_name',
        'premise',
        'thoroughfare',
        'dependent_locality',
        'postal_code',
        'locality',
        'country',
      ]),
      '#default_value' => $settings['fields'],
    ];
  }

  return $element;
}

/**
 * Render any address item.
 */
function _latlonfield_address_render(array $item, $settings, $uppercase = []) {

  $build = [];
  $build['#prefix'] = '<address>';
  $build['#suffix'] = '</address>';

  // Replace country name with full name.
  if (isset($item['country'])) {
    $countries = _addressfield_country_options_list();
    $item['country'] = $countries[$item['country']];
  }

  // Drop empty fields.
  foreach ($item as $key => $value) {
    if (empty($value)) {
      unset($item[$key]);
    } else if (in_array($key, $uppercase)) {
      $item[$key] = strtoupper(trim($value));
    }
  }

  // Filter out hidden fields
  if (!empty($settings['fields'])) {
    foreach ($item as $key => $value) {
      if (!in_array($key, $settings['fields'])) {
        unset($item[$key]);
      }
    }
  }

  // Group postal code and locality name.
  if (isset($item['postal_code']) && isset($item['locality'])) {
    $item['postal_code'] .= ' ' . $item['locality'];
    unset($item['locality']);
  }

  // WHAT WHAT? Sad Drupal is sad...
  $item = array_reverse($item, true);
  $build['#markup'] = implode("<br/>\n", $item);

  return $build;
}

/**
 * Render any address item using the french SNA/AFNOR standard.
 */
function _latlonfield_address_render_afnor(array $item, $settings) {
  return _latlonfield_address_render($item, $settings, ['country', 'postal_code', 'locality', 'dependent_locality']);
}

/**
 * Implements hook_field_formatter_view().
 */
function latlonfield_field_formatter_view($entity_type, $entity, $field, $instance, $langcode, $items, $display) {
  $element = [];

  switch ($display['type']) {

    case 'latlonfield':
      foreach ($items as $delta => $item) {
        $element[$delta] = [
          '#theme'  => 'latlonfield_default_formatter',
          '#item'   => $item,
        ];
      }
      break;

    case 'latlonfield_gmap':
      $element[0] = [
        '#theme'    => 'latlonfield_gmap_formatter',
        '#items'    => $items,
        '#api_key'  => variable_get('latlonfield_google_api_key'),
      ];
      break;

    case 'latlonfield_leaflet':
      // @todo
      break;

    case 'address':
      foreach ($items as $delta => $item) {
        $element[$delta] = _latlonfield_address_render($item, $display['settings'], []);
      }
      break;

    case 'address_fr_afnor':
      foreach ($items as $delta => $item) {
        $element[$delta] = _latlonfield_address_render_afnor($item, $display['settings']);
      }
      break;
  }

  return $element;
}

/**
 * Returns HTML for a default latlon field formatter.
 */
function theme_latlonfield_default_formatter($variables) {
  $item = $variables['item'];
  $output = '';

  $output .= '<div>';
  $output .= '<div class="latitude">' . $item['latitude'] . '</div>';
  $output .= '<div class="longitude">' . $item['longitude'] . '</div>';
  $output .= '</div>';

  return $output;
}

/**
 * Implements hook_process_THEME().
 */
function template_process_latlonfield_gmap_formatter(&$variables) {
  $items = $variables['items'];
  if (!empty($items)) {

    $api_key = $variables['api_key'];
    $map_options = $variables['map_options'];
    $marker_options = $variables['marker_options'];
    $attributes =& $variables['attributes'];

    if (empty($attributes['id'])) {
      $attributes['id'] = drupal_html_id('latlonfield-gmap');
    }

    $url = ($GLOBALS['is_https'] ? 'https' : 'http') . '://maps.google.com/maps/api/js?sensor=false' . ($api_key ? "&key=$api_key" : '');
    drupal_add_js($url, 'external');
    drupal_add_js(drupal_get_path('module', 'latlonfield') . '/latlonfield.js');
    drupal_add_js(array(
      'LatLonField' => array(
        'Gmap' => array(
          'id' => $attributes['id'],
          'items' => $items,
          'map_options' => (object)$map_options,
          'marker_options' => (object)$marker_options,
        ),
      ),
    ), 'setting');
  }
}

/**
 * Returns HTML for a google map latlon field formatter.
 */
function theme_latlonfield_gmap_formatter($variables) {
  return '<div' . drupal_attributes($variables['attributes']) . '></div>';
}
