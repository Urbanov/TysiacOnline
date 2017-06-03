var controller;

$(document).ready(function () {
	controller = new Controller();
	controller.openConnection("192.168.43.124", 2137);
	controller.view.initiateEventListeners();
});