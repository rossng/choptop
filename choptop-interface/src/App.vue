<template>
  <div id="app">
    <NavButton direction="up" :pressed="latestUpdate.upPressed" :eventBus="eventBus"/>
    <NavButton direction="left" :pressed="latestUpdate.leftPressed" :eventBus="eventBus"/>
    <div id="main">
      <WelcomeToChopTop v-if="shouldShowWelcome()"/>
      <PortionSelector v-if="shouldShowPortionSelector()" :eventBus="eventBus" :portionCount="portionCount"/>
      <RecipeListing v-if="shouldShowRecipeListing()" :recipes="recipe" :eventBus="eventBus" :portionCount="portionCount"/>
      <WeightDisplay :eventBus="eventBus"/>
    </div>
    <NavButton direction="right" :pressed="latestUpdate.rightPressed" :eventBus="eventBus"/>
    <NavButton direction="down" :pressed="latestUpdate.downPressed" :eventBus="eventBus"/>
  </div>

</template>

<script>
  import WelcomeToChopTop from './components/WelcomeToChopTop'
  import NavButton from './components/NavButton'
  import RecipeListing from './components/RecipeListing'
  import WeightDisplay from './components/WeightDisplay'
  import PortionSelector from './components/PortionSelector'
  import Timer from './components/Timer'

  import VueWebsocket from 'vue-native-websocket'

  import Vue from 'vue'
  import 'vue-awesome/icons'
  import Icon from 'vue-awesome/components/Icon'

  Vue.component('icon', Icon)

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
      RecipeListing,
      WeightDisplay,
      Timer,
      PortionSelector
    },

    data() {
      return {
        currentScreen: "welcome",
        recipe: Recipes,
        latestUpdate: {},
        focussed: true,
        eventBus : new Vue(),
        portionCount : 2,

      }
    },

    created () {
        this.setupFetchData();
        this.eventBus.$on("pressed", this.handlePress);
        this.eventBus.$on("portionCount",this.setPortionCount);
    },
    


    methods: {
      handlePress(dir){
        if (this.focussed = true){
          if(dir == "down"){
            this.nextState()
          }
        }
      },
      setPortionCount(count){
        this.portionCount = count;
      },
      upPressed(){
        this.prevState();
      },
      downPressed(){
        this.nextState();
      },
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
      shouldShowPortionSelector: function () {
        return this.currentScreen === "portionSelector";
      },
      nextState(){
        if(this.currentScreen === "portionSelector"){
          this.currentScreen = "recipeListing";
          this.focussed = false;
        }if(this.currentScreen ==="welcome"){
          this.currentScreen = "portionSelector"
          this.focussed = false;
        }
      },
      prevState(){
        if(this.currentScreen === "portionSelector"){
          this.currentScreen = "welcome";
          this.focussed = true;
        }else if(this.currentScreen === "recipeListing"){
          this.currentScreen = "portionSelector"
        }
      },
      handleNewData(msg){
        var data = msg.data
        console.log(data)
        parsed = {};
        try{
          var parsed = JSON.parse(data)
        }catch (e)
        {
          console.log("bad Json ")
          console.log(msg)
        }

        if(parsed.event =="leftPressed"){
          this.eventBus.$emit("pressed",'left')
        }
        if(parsed.event =="rightPressed"){
          this.eventBus.$emit("pressed",'right')
        }
        if(parsed.event =="upPressed"){
          this.eventBus.$emit("pressed",'up')
        }
        if(parsed.event =="downPressed"){
          this.eventBus.$emit("pressed",'down')
        }
        if(parsed.event == "weightReading"){
          this.eventBus.$emit("weight", parsed.value)
        }
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
    height: 440px; /*480px - 2 x 20px */
    float: left;
    display: inline-block;
    background: #f2b74b;
    /*background:white;*/
  }


</style>
