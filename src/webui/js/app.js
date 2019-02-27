(function () {
    'use strict';

    angular.module('Chameleon', ['ngRoute','ui.bootstrap'])
    //配置$routeProvider用来定义路由规则
    //$routeProvider为我们提供了when（path，object）& other(object)函数按顺序定义所有路由，函数包含两个参数：
    //@param1:url或者url正则规则
    //@param2：路由配置对象
        .config(function ($routeProvider) {
            $routeProvider.when('/hardware', {
                //templateURL：插入ng-view的HTML模板文件
                templateUrl: '/Chameleon/src/webui/HTML/hardware.html',
                controller: 'HardwareCtrl'

            })
                .when('/runtime', {
                    templateUrl: '/Chameleon/src/webui/HTML/runtime.html',
                    controller: 'RuntimeCtrl'
                })
                // network_topology.html
                .when('/topology', {
                    templateUrl: '/Chameleon/src/webui/HTML/network_topology.html',
                    controller: 'TopologyCtrl'
                })

                // supernetwork_topology.html
                .when('/topology/super_master', {
                    templateUrl: '/Chameleon/src/webui/HTML/supernetwork_topology.html',
                    controller: 'SuperTopologyCtrl'
                })
                // frameworks.html
                .when('/frameworks',{
                    templateUrl:
 '/Chameleon/src/webui/HTML/frameworks.html',
                    controller:'FrameworkCtrl'
                })

                // framework.html
                .when('/frameworks/:frameworksID',{
            templateUrl:
                '/Chameleon/src/webui/HTML/framework.html',
                    controller:'FrameworkCtrl'
            })
                // slave.html
                .when('/slaves/:slave_uuid/ip/:slave_id/cpu_usage/:cpu_used/mem_usage/:mem_used/disk_usage/:disk_used', {
                    templateUrl: '/Chameleon/src/webui/HTML/slave.html',
                    controller: 'SlaveCtrl'
                })
        })


})();