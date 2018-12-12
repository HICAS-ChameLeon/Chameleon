var app = angular.module('myApp', []);

// app.controller('siteCtrl', function($scope, $http) {
//     $http.get("http://172.20.110.228/master/runtime-resources")
//         .then(function (response) {$scope.names = response.data.quantity;});
// });

app.controller('hardware', function($scope, $http) {
    $http({
        method: 'GET',
        url: 'http://172.20.110.228:6060/master/hardware-resources'
    }).then(function successCallback(response) {
        $scope.hardware = response.data.content;
        console.log(response.data.content);
        $scope.quantities = response.data.quantity;
        console.log(response.data.quantity);
    }, function errorCallback(response) {
        // 请求失败执行代码
    });
});

var runtimeApp = angular.module('runtimeApp', []);
runtimeApp.controller('runtimeCtrl', function($scope, $http) {
    $http({
        method: 'GET',
        url: 'http://172.20.110.228:6060/master/runtime-resources'
    }).then(function successCallback(response) {
        $scope.runtime = response.data.content;
        console.log(response.data.content);
        $scope.quantities = response.data.quantity;
        console.log(response.data.quantity);
    }, function errorCallback(response) {
        // 请求失败执行代码
    });
});

//手动加载ng-app
angular.bootstrap(document.querySelector('[ng-app="runtimeApp"]'), ['runtimeApp']);
