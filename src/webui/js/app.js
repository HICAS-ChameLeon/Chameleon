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
                // slave.html
                .when('/slaves/:slave_uuid/ip/:slave_id/cpu_usage/:cpu_used/mem_usage/:mem_used/disk_usage/:disk_used', {
                    templateUrl: '/Chameleon/src/webui/HTML/slave.html',
                    controller: 'SlaveCtrl'
                })
        })

    // angular.module('Chameleon', ['ngAnimate', 'ngSanitize', 'ui.bootstrap']);
    // angular.module('Chameleon').controller('ModalDemoCtrl', function ($modal, $log) {
    //     var pc = this;
    //     pc.data = "Lorem Name Test";
    //
    //     pc.open = function (size) {
    //         var modalInstance = $modal.open({
    //             animation: true,
    //             ariaLabelledBy: 'modal-title',
    //             ariaDescribedBy: 'modal-body',
    //             templateUrl: '/Chameleon/src/webui/HTML/modal.html',
    //             controller: 'ModalInstanceCtrl',
    //             controllerAs: 'pc',
    //             //backdrop: true,
    //             size: size,
    //             resolve: {
    //                 data: function () {
    //                     return pc.data;
    //                 }
    //             }
    //         });
    //
    //         modalInstance.result.then(function () {
    //             alert("now I'll close the modal");
    //         });
    //     };
    // });
    //
    // angular.module('Chameleon').controller('ModalInstanceCtrl', function ($modalInstance, data) {
    //     var pc = this;
    //     pc.data = data;
    //     console.log(55 +data)
    //
    //     pc.ok = function () {
    //         //{...}
    //         alert("You clicked the ok button.");
    //         $modalInstance.close();
    //     };
    //
    //     pc.cancel = function () {
    //         //{...}
    //         alert("You clicked the cancel button.");
    //         $modalInstance.dismiss('cancel');
    //     };
    // });


})();