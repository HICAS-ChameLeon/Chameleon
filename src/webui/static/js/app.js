/*
 * Copyright  ：SIAT 异构智能计算体系结构与系统研究中心
 * Author     ：Jessicalllo  sherlock_vip@163.com
 * Date       ：18-11-26
 * Description：app.js
 */

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
                //controller: 控制与templateUrl相对应的html的控制器
                templateUrl: 'hardware.html',
                controller: 'HardwareCtrl'

            })
                .when('/runtime', {
                    templateUrl: 'runtime.html',
                    controller: 'UpdateCtrl'
                })

                .when('/scheduling', {
                    templateUrl: 'scheduler.html',
                    controller: 'SchedulerCtrl'
                })
                // network_topology.html
                .when('/topology', {
                    templateUrl: 'network_topology.html',
                    controller: 'TopologyCtrl'
                })

                // supernetwork_topology.html
                .when('/topology/super_master', {
                    templateUrl: 'supernetwork_topology.html',
                    controller: 'SuperTopologyCtrl'
                })
                .when('/topology/super_master_and_super_master', {
                    templateUrl: 'super_and_supernetwork_topology.html',
                    controller: 'SuperAndSuperTopologyCtrl'
                })
                //framework_topology.html
                .when('/framework_topology',{
                    templateUrl:'framework_topology.html',
                    controller: 'FrameworkTopologyCtrl'
                })
                // frameworks.html
                .when('/frameworks',{
                    templateUrl: 'frameworks.html',
                    controller:'FrameworkCtrl'
                })

                // framework.html
                .when('/frameworks/:framework_uuid', {
                    templateUrl: 'framework.html',
                    controller: 'FrameworkCtrl'
                })
                // slave.html
                .when('/slaves/:slave_uuid/ip/:slave_id/cpu_usage/:cpu_used/mem_usage/:mem_used/disk_usage/:disk_used', {
                    templateUrl: 'slave.html',
                    controller: 'SlaveCtrl'
                })
        })
        //指令clipboard,实现点击复制
        .directive('clipboard', [function() {
            return {
                restrict: 'A',
                scope: true,
                template: '<i class="glyphicon glyphicon-file"></i>',

                link: function(scope, element, attrs) {
                    var clip = new Clipboard(element[0]);

                    element.on('mouseenter', function() {
                        element.addClass('clipboard-is-hover');
                        element.triggerHandler('clipboardhover');
                    });

                    element.on('mouseleave', function() {
                        // Restore tooltip content to its original value if it was
                        // changed by this Clipboard instance.
                        if (scope && scope.tt_content_orig) {
                            scope.tt_content = scope.tt_content_orig;
                            delete scope.tt_content_orig;
                        }

                        element.removeClass('clipboard-is-hover');
                        element.triggerHandler('clipboardhover');
                    });

                    // Success for browsers with `execCommand` support.
                    // 兼容execCommand方法的浏览器
                    clip.on('success', function () {
                        // Store the tooltip's original content so it can
                        // be restored when the tooltip is hidden.
                        scope.tt_content_orig = scope.tt_content;

                        // Angular UI's Tooltip sets content on the element's scope in a
                        // variable named 'tt_content'. The Tooltip has no public interface,
                        // so set the value directly here to change the value of the tooltip
                        // when content is successfully copied.
                        scope.tt_content = '已复制';
                        scope.$apply();
                    });

                    // Support for all other browsers without `execCommand`  support.
                    // 不兼容execCommand方法的浏览器
                    // Text will be selected and user will be prompted to copy.
                    // 将选择文本并提示用户复制
                    clip.on('error', function() {
                        scope.tt_content_orig = scope.tt_content;
                        scope.tt_content = 'Press Ctrl/Cmd + C to copy!';
                        scope.$apply();
                    });
                }
            };
        }])

})();