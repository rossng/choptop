<template>
  <div id="app">
    <NavButton direction="up" :pressed="latestUpdate.upPressed"/>
    <NavButton direction="left" :pressed="latestUpdate.leftPressed"/>
    <div id="main">
      <WelcomeToChopTop v-if="shouldShowWelcome()"/>
      <RecipeListing v-if="shouldShowRecipeListing()" :recipes="recipe"/>
    </div>
    <NavButton direction="right" :pressed="latestUpdate.rightPressed"/>
    <NavButton direction="down" :pressed="latestUpdate.downPressed"/>
    <button v-on:click="leftPressed = !leftPressed">toggleLeft</button>
  </div>

</template>

<script>
  import WelcomeToChopTop from './components/WelcomeToChopTop'
  import NavButton from './components/NavButton'
  import RecipeListing from './components/RecipeListing'

  import VueWebsocket from 'vue-native-websocket'

  import Vue from 'vue'

  Vue.use(VueWebsocket, "ws://localhost:8765", { 
      format: 'json',
      reconnection: true, // (Boolean) whether to reconnect automatically (false)
      reconnectionAttempts: 10000, // (Number) number of reconnection attempts before giving up (Infinity),
      reconnectionDelay: 3000, // (Number) how long to initially wait before attempting a new (1000)
    });


  // import recipes json file
  import Recipes from './recipes.json'

  export default {
    name: 'app',
    components: {
      WelcomeToChopTop,
      NavButton,
      RecipeListing
    },

    data() {
      return {
        currentScreen: "recipeListing",
        recipe: Recipes,
        latestUpdate: {}

      }
    },

    created () {
      // fetch the data when the view is created and the data is
      // already being observed
      this.setupFetchData()
    },


    methods: {
      setupFetchData () {
      //results from websocket connection
        this.$options.sockets.onmessage = this.handleNewData
      },
      shouldShowWelcome: function () {
        return this.currentScreen === "welcome"

      },
      shouldShowRecipeListing: function () {
        return this.currentScreen === "recipeListing"
      },
      handleNewData(msg){
        var data = msg.data
        console.log(data)
        this.latestUpdate = JSON.parse(data)
      }
    }
  }

</script>
<style>

  #app {
    font-family: 'Avenir', Helvetica, Arial, sans-serif;
    -webkit-font-smoothing: antialiased;
    -moz-osx-font-smoothing: grayscale;
    text-align: center;
    color: #2c3e50;
    height: 100%;
    width: 100%;
    background: #026967;
    /*overflow:hidden;*/
  }

  #main {
    width: 760px;
    height: 460px;
    float: left;
    display: inline-block;
    background: #f2b74b;
  }


</style>
