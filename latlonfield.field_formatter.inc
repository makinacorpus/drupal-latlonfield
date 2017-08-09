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
      'settings'    => [
        'id'          => null,
        'map'         => [],   // Use this to arbitrary set map options
        'zoom'        => null, // Default zoom
        'maxZoom'     => null, // Maximum zoom
        'minZoom'     => null, // Minimum zoom
        'latitude'    => null, // Default center latitude
        'longitude'   => null, // Default center longitude
        'showBubbles' => false,
        'attribution' => 'Map data © <a href="http://openstreetmap.org">OpenStreetMap</a> contributors',
      ],
    ],
    'address' => [
      'label'       => t('Default'),
      'field types' => ['address'],
      'settings'    => [
        'fields' => [],
        'notag'  => FALSE,
      ],
    ],
    'address_fr_afnor' => [
      'label'       => t('French SNA/AFNOR standard'),
      'field types' => ['address'],
      'settings'    => [],
    ],
    'google_maps_url' => [
      'label'       => t('Google maps URL'),
      'field types' => ['latlon'],
      'settings'    => [
        'title'       => 'Google Maps',
        'attributes'  => [],
      ],
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

  if ($display['type'] == 'address' || $display['type'] == 'address_fr_afnor') {
    $element['notag'] = [
      '#title'          => t('Do not wrap with an &lt;address&gt; tag'),
      '#type'           => 'checkbox',
      '#default_value'  => $settings['notag'],
    ];

    $element['md_enabled'] = [
      '#title'          => t('Automatically add microdata'),
      '#type'           => 'checkbox',
      '#default_value'  => $settings['md_enabled'],
    ];

    $element['md_property'] = [
      '#title'          => t('Microdata property name'),
      '#type'           => 'textfield',
      '#description'    => t('Fill in this field if you want to define the address as a property of another microdata type. Microdata must be enabled and the wrapper option must be unchecked.'),
      '#default_value'  => $settings['md_property'],
//      '#states' => [
//        'enabled' => [
//          ':input[name*="md_enabled"]' => ['checked' => true],
//          ':input[name*="notag"]'      => ['unchecked' => true],
//        ],
//        'disabled' => [
//          [':input[name*="md_enabled"]' => ['unchecked' => true]],
//          [':input[name*="notag"]'      => ['checked' => true]],
//        ],
//      ],
    ];
  }

  return $element;
}

/**
 * Implements hook_field_formatter_settings_summary().
 */
function latlonfield_field_formatter_settings_summary($field, $instance, $view_mode) {
  $display = $instance['display'][$view_mode];
  $settings = $display['settings'];

  $summary = [];

  if ($display['type'] == 'address' && !empty($settings['fields'])) {
    $summary[] = t('Fields displayed: !fields', ['!fields' => implode(', ', $settings['fields'])]);
  }

  if ($display['type'] == 'address' || $display['type'] == 'address_fr_afnor') {
    if (isset($settings['notag']) && $settings['notag']) {
      $summary[] = t('No &lt;address&gt; tag wrapping the address');
    }

    if (isset($settings['md_enabled']) && $settings['md_enabled']) {
      $summary[] = t('Microdata enabled');
      if (empty($settings['notag']) && !empty($settings['md_property'])) {
        $summary[] = t('Microdata property name: @name', ['@name' => $settings['md_property']]);
      }
    } else {
      $summary[] = t('Microdata disabled');
    }
  }

  if (empty($summary)) {
    $summary[] = t('Display by default');
  }

  return implode('<br/>', $summary);
}

/**
 * Render any address item.
 */
function _latlonfield_address_render(array $item, $settings, $uppercase = []) {
  $build = [];

  if (!isset($settings['notag']) || !$settings['notag']) {
    $itemprop = $itemscope = '';

    if (isset($settings['md_enabled']) && $settings['md_enabled']) {
      $itemscope = ' itemscope itemtype="http://schema.org/PostalAddress"';
      if (!empty($settings['md_property'])) {
        $itemprop = ' itemprop="' . $settings['md_property'] . '"';
      }
    }

    $build['#prefix'] = '<address' . $itemprop . $itemscope . '>';
    $build['#suffix'] = '</address>';
  }

  // Replace country name with full name.
  if (isset($item['country'])) {
    $item['country'] = t('France');
  }

  // Drop empty fields.
  foreach ($item as $key => $value) {
    if (empty($value)) {
      unset($item[$key]);
    } else if (in_array($key, $uppercase)) {
      $item[$key] = strtoupper(trim($value));
    }
  }

  // Prepare the address fields
  foreach ($item as $key => $value) {
    // Filter out hidden fields
    if (!empty($settings['fields']) && !in_array($key, $settings['fields'])) {
      unset($item[$key]);
      continue;
    }

    $attributes = ['class' => drupal_clean_css_identifier($key)];

    // Add microdata if enabled
    if (isset($settings['md_enabled']) && $settings['md_enabled']) {
      switch ($key) {
        case 'name_line':     $attributes['itemprop'] = 'name';             break;
        case 'premise':       $attributes['itemprop'] = 'streetAddress';    break;
        case 'thoroughfare':  $attributes['itemprop'] = 'streetAddress';    break;
        case 'postal_code':   $attributes['itemprop'] = 'postalCode';       break;
        case 'locality':      $attributes['itemprop'] = 'addressLocality';  break;
        case 'country':       $attributes['itemprop'] = 'addressCountry';   break;
      }
    }

    $item[$key] = '<span' . drupal_attributes($attributes) . '>' . $value . '</span>';
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
 * Render Google Maps URL.
 */
function _latlonfield_address_render_google_maps_url(array $item, $settings) {
  return ['#markup' => l($settings['title'],  'https://www.google.com/maps/place/' . $item['latitude'] . ',' . $item['longitude'], ['attributes' => $settings['attributes']])];
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
        '#options'  => $display['settings'],
        '#api_key'  => variable_get('latlonfield_google_api_key'),
      ];
      break;

    case 'latlonfield_leaflet':
      $element[0] = [
        '#theme'    => 'latlonfield_leaflet_formatter',
        '#options'  => $display['settings'],
        '#items'    => $items,
      ];
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

    case 'google_maps_url':
      foreach ($items as $delta => $item) {
        $element[$delta] = _latlonfield_address_render_google_maps_url($item, $display['settings']);
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

    $api_key        = $variables['api_key'];
    $map_options    = $variables['map_options'];
    $marker_options = $variables['marker_options'];
    $attributes     = &$variables['attributes'];

    if (empty($attributes['id'])) {
      $attributes['id'] = drupal_html_id('latlonfield-gmap');
    }

    drupal_add_js(LATLONFIELD_GMAP_API . ($api_key ? "&key=$api_key" : ''), 'external');
    drupal_add_js(drupal_get_path('module', 'latlonfield') . '/latlonfield.gmap.js');
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
 * Implements hook_process_THEME().
 */
function template_process_latlonfield_leaflet_formatter(&$variables) {

  $items    = $variables['items'];
  $options  = $variables['options'];

  if (!empty($items)) {

    $options += [
      'showBubbles' => false,
      'items' => $items
    ];

    // Fixes maybe $options identifier.
    if (empty($options['id'])) {
      $options['id'] = drupal_html_id('latlonfield-leaflet');
    }

    // Add necessary JS and prey for it to work.
    drupal_add_js(LATLONFIELD_LEAFLET_CDN_JS, 'external');
    drupal_add_css(LATLONFIELD_LEAFLET_CDN_CSS, 'external');
    drupal_add_js(drupal_get_path('module', 'latlonfield') . '/latlonfield.leaflet.js');
    drupal_add_js(array(
      'LatLonField' => array(
        'Leaflet' => array(
           $options,
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

/**
 * Returns HTML for a google map latlon field formatter.
 */
function theme_latlonfield_leaflet_formatter($variables) {
  return '<div id="' . $variables['options']['id'] . '"></div>';
}
