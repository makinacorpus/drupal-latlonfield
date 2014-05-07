<?php

/**
 * @file
 * Implement a latlon field.
 */

/**
 * Implements hook_field_info().
 */
function latlonfield_field_info() {
  return array(
    'latlon' => array(
      'label' => t('Lat/Lon'),
      'description' => t('This field stores a latitude and a longitude.'),
      'settings' => array(),
      'instance_settings' => array(),
      'default_widget' => 'latlon',
      'default_formatter' => 'latlon',
    ),
  );
}

/**
 * Implements hook_field_is_empty().
 */
function latlonfield_field_is_empty($item, $field) {
  return empty($item['latitude']) || empty($item['longitude']);
}

/**
 * Implements hook_field_widget_info().
 */
function latlonfield_field_widget_info() {
  return array(
    'latlon' => array(
      'label' => t('Default'),
      'field types' => array('latlon'),
      'settings' => array(),
    ),
  );
}

/**
 * Implements hook_field_widget_form().
 */
function latlonfield_field_widget_form(&$form, &$form_state, $field, $instance, $langcode, $items, $delta, $element) {

  $element += array(
    '#type' => 'fieldset',
    '#title' => t('Latitude/Longitude'),
    '#collapsible' => FALSE,
    '#element_validate' => array('latlonfield_element_validate'),
  );
  $element['latitude'] = array(
    '#type' => 'textfield',
    '#title' => t('Latitude'),
    '#default_value' => isset($items[$delta]['latitude']) ? $items[$delta]['latitude'] : '',
    '#element_validate' => array('element_validate_number'),
    '#pre_render' => array('latlonfield_element_required'),
  );
  $element['longitude'] = array(
    '#type' => 'textfield',
    '#title' => t('Longitude'),
    '#default_value' => isset($items[$delta]['longitude']) ? $items[$delta]['longitude'] : '',
    '#element_validate' => array('element_validate_number'),
    '#pre_render' => array('latlonfield_element_required'),
  );

  return $element;
}

/**
 * Pre-render element callback to theme required element.
 */
function latlonfield_element_required($element) {
  $element['#required'] = TRUE;
  return $element;
}

/**
 * Address element validate callback.
 */
function latlonfield_element_validate($element, &$form_state, $form) {

  if (!empty($element['latitude']['#value']) && empty($element['longitude']['#value'])) {
    form_error($element['longitude'], t('Field %name is required.', array('%name' => $element['longitude']['#title'])));
  }
  elseif (!empty($element['latitude']['#value']) && empty($element['longitude']['#value'])) {
    form_error($element['latitude'], t('Field %name is required.', array('%name' => $element['latitude']['#title'])));
  }
}

/**
 * Implements hook_field_formatter_info().
 */
function latlonfield_field_formatter_info() {
  $formatters = array(
    'latlonfield' => array(
      'label' => t('Default'),
      'field types' => array('latlon'),
      'settings' => array(),
    ),
    'latlonfield_gmap' => array(
      'label' => t('Google map'),
      'field types' => array('latlon'),
      'settings' => array('google_api_key' => ''),
    ),
  );

  return $formatters;
}

/**
 * Implements hook_field_formatter_settings_form().
 */
function latlonfield_field_formatter_settings_form($field, $instance, $view_mode, $form, &$form_state) {
  $type = $instance['display'][$view_mode]['type'];
  $settings = $instance['display'][$view_mode]['settings'];
  $element = array();

  if ($type === 'latlonfield_gmap') {
    $element['google_api_key'] = array(
      '#type' => 'textfield',
      '#title' => t('Google map API key'),
      '#description' => t('Generic Google API key could be set at this <a href="!url">page</a>.', array('!url' => url('admin/config/content/latlonfield'))),
      '#default_value' => !empty($settings['google_api_key']) ? $settings['google_api_key'] : variable_get('latlonfield_google_api_key', ''),
    );
  }

  return $element;
}

/**
 * Implements hook_field_formatter_settings_summary().
 */
function latlonfield_field_formatter_settings_summary($field, $instance, $view_mode) {
  $display = $instance['display'][$view_mode];
  $settings = $display['settings'];
  $output = '';

  if ($display['type'] === 'latlonfield_gmap') {
    $output .= t('API key : !key', array('!key' => !empty($settings['google_api_key']) ? $settings['google_api_key'] : t('None')));
  }
  return $output;
}

/**
 * Implements hook_field_formatter_view().
 */
function latlonfield_field_formatter_view($entity_type, $entity, $field, $instance, $langcode, $items, $display) {
  $element = array();

  if ($display['type'] === 'latlonfield') {
    foreach ($items as $delta => $item) {
      $element[$delta] = array(
        '#theme' => 'latlonfield_default_formatter',
        '#item' => $item,
      );
    }
  }
  elseif ($display['type'] === 'latlonfield_gmap') {
    $element = array(
      '#theme' => 'latlonfield_gmap_formatter',
      '#items' => $items,
      '#api_key' => $display['settings']['google_api_key'],
    );
  }

  return $element;
}

/**
 * Returns HTML for a default latlon field formatter.
 *
 * @ingroup themeable
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
 *
 * @ingroup themeable
 */
function theme_latlonfield_gmap_formatter($variables) {
  return '<div' . drupal_attributes($variables['attributes']) . '></div>';
}
