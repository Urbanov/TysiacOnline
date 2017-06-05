var controller;

$(document).ready(function () {
	controller = new Controller();
	controller.openConnection(window.location.hostname, 2137);
	controller.view.initiateEventListeners();
});