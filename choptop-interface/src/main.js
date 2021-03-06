// The Vue build version to load with the `import` command
// (runtime-only or standalone) has been set in webpack.base.conf with an alias.
import Vue from 'vue'
import App from './App'

Vue.config.productionTip = false;

require('../static/globalStyles.css');

/* eslint-disable no-new */
let app = new Vue({
  el: '#app',
  template: '<App/>',
  components: { App }
});

window.addEventListener('keypress', function(e) {
  app.$children[0].handleGlobalKeyDown(e);
});
