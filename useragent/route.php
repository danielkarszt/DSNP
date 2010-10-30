<?php
$USER_NAME = null;

if ( isset( $_GET['url'] ) )
	$url = $_GET['url'];

if ( !isset( $url ) ) {
	# If there is no URL then default to site/index
	$route = array( 'site', 'index' );
}
else {
	# Split on '/'.
	$route = explode( '/', $url );

	# Validate the component names.
	$normalizedRoute = array();
	foreach ( $route as $component ) {
		if ( $component === "" )
			continue;
		else if	( ! preg_match( '/^[a-zA-Z][a-zA-Z0-9_\-]*$/', $component ) )
			die( "route component '$component' does not validate" );
		else {
			$normalizedRoute[] = $component;
		}
	}
	$route = $normalizedRoute;

	# If the first element of the route is anything but 'site', then it is a
	# user. Shift the array to get the controller at the head.
	if ( $route[0] !== 'site' )
		$USER_NAME = array_shift( $route );

	# If there is no function then default it to index.
	if ( !isset( $route[0] ) )
		$route[0] = 'index';

	# If there is no function then default it to index.
	if ( !isset( $route[1] ) )
		$route[1] = 'index';
}

foreach ( $route as $component ) {
	# Derive a class name by stripping out underscores and dashes. We can
	# expect this to be non-empty because the above regex requires alpha as the
	# first character.
	$className[$component] = preg_replace( '/[_\-]+/', '', $component );
}

# Check for the controller file.
$controllerFile = ROOT . DS . 'controller' . DS . $route[0] . '.php';
if ( !file_exists( $controllerFile ) )
	die( "invalid URL: controller file '$controllerFile' does not exist" );

# Make sure the controller class does not exist already.
$controllerName = $className[$route[0]];
$controllerClassName = $controllerName . "Controller";
if ( class_exists( $controllerClassName ) )
	die("internal error: controller '$controllerClassName' is prexisting");

# Include the file and make sure we have the class now.
include( $controllerFile );

if ( ! class_exists( $controllerClassName ) ) {
	die("internal error: controller '$controllerClassName' class " . 
		"not defined in $controllerFile");
}

# Allocate controller.
$controller = new $controllerClassName;
if ( !is_object( $controller ) )
	die("internal error: failed to allocate controller");

# Find method
$methodName = $className[$route[1]];
if ( !method_exists( $controller, $methodName ) )
	die("invalid URL: method $methodName not present in $controllerClassName");

$controller->controller = $controllerName;
$controller->method = $methodName

?>
