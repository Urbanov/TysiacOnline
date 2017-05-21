// global variable
var controller;

$(document).ready(function () {
	controller = new Controller();
	controller.openConnection("127.0.0.1", 2137);
	controller.view.initiateEventListeners();
});