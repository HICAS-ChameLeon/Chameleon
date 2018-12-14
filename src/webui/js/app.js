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
        //
        //
        // // $scope.quantities = response.data.quantity;
        // console.log(response.data.quantity);
        //
        //
        // var my_master_json = $scope.hardware;
        // var my_master = {};
        // my_master.hostname = "rrr";
        // my_master.pid = my_master_json.slave_id;
        // //my_master.port = my_master_json['port'];
        // my_master.id = 1;
        // my_master.label = my_master.pid;
        // my_master.shape = 'image';
        // my_master.image = DIR + 'Network-Pipe-icon.png';
        // my_master.title = '主节点';
        //
        // my_slaves[0] = my_master;
        // var index_slave = 1;
        // var index_edge = 0;
        // var my_edges = [];
        // // if($scope.quantities>=2){
        // //     my_edges = [$scope.quantities-1];
        // // }
        // // var temp_slave = {};
        // // temp_slave.hostname = slave['hostname'];
        // // temp_slave.pid = slave['pid'];
        // // temp_slave.id = 23;
        // // temp_slave.label = 12344;
        // // temp_slave.shape ='';
        // var nodes = new vis.DataSet(my_slaves);
        //
        // // console.log(my_slaves[0]);
        // // $scope.myname = $scope.agents;
        // // alert("www"+$scope.agents);
        //
        // // create an array with edges
        // var edges = new vis.DataSet(my_edges);
        //
        // var container = document.getElementById('mynetwork');
        // var data = {
        //     nodes: nodes,
        //     edges: edges
        // };
        //
        // var options = {
        //     interaction:{hover:true},
        //     manipulation: {
        //         enabled: true
        //     }
        // };
        //
        // network = new vis.Network(container, data, options);

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
        console.log(79);
        $scope.runtime = response.data.content;
        console.log(response.data.content);
        $scope.quantities = response.data.quantity;
        console.log(response.data.quantity);

        var DIR = '../icon/refresh-cl/';
        console.log($scope.quantities);
        var my_slaves = new Array(1);
        console.log("21");

        // for (var i=0,len=$scope.runtime.length; i<len; i++)
        // {
        //     console.log(response.data.content[i]);
        // }
        for(var i in $scope.runtime){
            console.log(i);
            console.log($scope.runtime[i]);
        }

        var my_master_json = $scope.hardware;
        var my_master = {};
        my_master.hostname = "rrr";
        my_master.pid = 556666;
        //my_master.port = my_master_json['port'];
        my_master.id = 1;
        my_master.label = my_master.pid;
        my_master.shape = 'image';
        my_master.image = DIR + 'Network-Pipe-icon.png';
        my_master.title = '主节点';

        my_slaves[0] = my_master;
        var index_slave = 1;
        var index_edge = 0;
        var my_edges = [];
        // if($scope.quantities>=2){
        //     my_edges = [$scope.quantities-1];
        // }
        // var temp_slave = {};
        // temp_slave.hostname = slave['hostname'];
        // temp_slave.pid = slave['pid'];
        // temp_slave.id = 23;
        // temp_slave.label = 12344;
        // temp_slave.shape ='';
        var nodes = new vis.DataSet(my_slaves);

        // console.log(my_slaves[0]);
        // $scope.myname = $scope.agents;
        // alert("www"+$scope.agents);

        // create an array with edges
        var edges = new vis.DataSet(my_edges);

        var container = document.getElementById('mynetwork');
        var data = {
            nodes: nodes,
            edges: edges
        };

        var options = {
            interaction:{hover:true},
            manipulation: {
                enabled: true
            }
        };

        network = new vis.Network(container, data, options);


    }, function errorCallback(response) {
        // 请求失败执行代码
    });
});

// var network = angular.module('network',[]);
// network.controller('NetworkCtrl',function ($scope) {
//     console.log(quantities);
//     var my_slaves = new Array(quantities);
//
//
//
//
// })

//手动加载ng-app
angular.bootstrap(document.querySelector('[ng-app="runtimeApp"]'), ['runtimeApp']);
