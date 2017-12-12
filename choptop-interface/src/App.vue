<template>
  <div id="app">
    <NavButton direction="up" :eventBus="eventBus"/>
    <NavButton direction="left" :eventBus="eventBus"/>
    <div id="main">
      <WelcomeToChopTop v-if="shouldShowWelcome()"/>
      <PortionSelector v-if="shouldShowPortionSelector()" :eventBus="eventBus" :portionCount="portionCount"/>
      <RecipeListing v-if="shouldShowRecipeListing()" :recipes="recipe" :eventBus="eventBus"
                     :portionCount="portionCount"/>
      <!-- <WeightDisplay :eventBus="eventBus"/> -->
      <TapPosition :eventBus="eventBus"/>
    </div>
    <NavButton direction="right" :eventBus="eventBus"/>
    <NavButton direction="down" :eventBus="eventBus"/>
  </div>

</template>

<script>
  import WelcomeToChopTop from './components/WelcomeToChopTop'
  import NavButton from './components/NavButton'
  import RecipeListing from './components/RecipeListing'
  import WeightDisplay from './components/WeightDisplay'
  import PortionSelector from './components/PortionSelector'
  import Timer from './components/Timer'
  import TapPosition from './components/TapPosition'

  import VueWebsocket from 'vue-native-websocket'

  import Vue from 'vue'
  import 'vue-awesome/icons'
  import Icon from 'vue-awesome/components/Icon'

  Vue.component('icon', Icon);

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
      PortionSelector,
      TapPosition,
    },

    data() {
      return {
        currentScreen: "welcome",
        recipe: Recipes,
        focussed: true,
        eventBus: new Vue(),
        portionCount: 2,

      }
    },

    created() {
      this.setupFetchData();
      this.eventBus.$on("pressed", this.handlePress);
      this.eventBus.$on("portionCount", this.setPortionCount);
    },


    methods: {
      handlePress(dir) {
        if (this.focussed = true) {
          if (dir === "down") {
            this.nextState()
          }
        }
      },
      setPortionCount(count) {
        this.portionCount = count;
      },
      upPressed() {
        this.prevState();
      },
      downPressed() {
        this.nextState();
      },
      setupFetchData() {
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
      nextState() {
        if (this.currentScreen === "portionSelector") {
          this.currentScreen = "recipeListing";
          this.focussed = false;
        }
        if (this.currentScreen === "welcome") {
          this.currentScreen = "portionSelector";
          this.focussed = false;
        }
      },
      prevState() {
        if (this.currentScreen === "portionSelector") {
          this.currentScreen = "welcome";
          this.focussed = true;
        } else if (this.currentScreen === "recipeListing") {
          this.currentScreen = "portionSelector"
        }
      },
      handleGlobalKeyDown: function (e) {
        switch (e.key) {
          case "ArrowUp":
            this.eventBus.$emit("pressed", 'up');
            break;
          case "ArrowRight":
            this.eventBus.$emit("pressed", 'right');
            break;
          case "ArrowDown":
            this.eventBus.$emit("pressed", 'down');
            break;
          case "ArrowLeft":
            this.eventBus.$emit("pressed", 'left');
            break;
          default:
            return;
        }
      },
      handleNewData(msg) {
        let data = msg.data;
        console.log(data);
        let parsed = {};
        try {
          parsed = JSON.parse(data)
        } catch (e) {
          console.log("Received invalid JSON from server");
          console.log(msg)
        }

        //left
        if (parsed.event === "leftPressed") {
          if (parsed.pressInfo === "start") {
            this.eventBus.$emit("pressStart", "left")
          } else if (parsed.pressInfo === "success") {
            this.eventBus.$emit("pressed", 'left')
          } else if (parsed.pressInfo === "cancel") {
            this.eventBus.$emit("pressCancel", 'left')
          }
        }

        //right
        if (parsed.event === "rightPressed") {
          if (parsed.pressInfo === "start") {
            this.eventBus.$emit("pressStart", "right")
          } else if (parsed.pressInfo === "success") {
            this.eventBus.$emit("pressed", 'right')
          } else if (parsed.pressInfo === "cancel") {
            this.eventBus.$emit("pressCancel", 'right')
          }
        }

        //up
        if (parsed.event === "upPressed") {
          if (parsed.pressInfo === "start") {
            this.eventBus.$emit("pressStart", "up")
          } else if (parsed.pressInfo === "success") {
            this.eventBus.$emit("pressed", 'up')
          } else if (parsed.pressInfo === "cancel") {
            this.eventBus.$emit("pressCancel", 'up')
          }
        }

        //down
        if (parsed.event === "downPressed") {
          if (parsed.pressInfo === "start") {
            this.eventBus.$emit("pressStart", "down")
          } else if (parsed.pressInfo === "success") {
            this.eventBus.$emit("pressed", 'down')
          } else if (parsed.pressInfo === "cancel") {
            this.eventBus.$emit("pressCancel", 'down')
          }
        }
        if (parsed.event === "weightReading") {
          this.eventBus.$emit("weight", parsed.value)
        }
        if (parsed.event === "position") {
          this.eventBus.$emit("pos", parsed)
        }
      }
    }
  }

</script>
<style>

  @import url('https://fonts.googleapis.com/css?family=Fredoka+One|Roboto');

  #app {
    font-family: 'Roboto', Helvetica, Arial, sans-serif;
    -webkit-font-smoothing: antialiased;
    -moz-osx-font-smoothing: grayscale;
    text-align: center;
    color: #4e3726;
    height: 100%;
    width: 100%;
    background: #7cff70;
  }

  #main {
    width: 760px;
    height: 440px; /*480px - 2 x 20px */
    float: left;
    display: inline-block;
    background: white;
  }

  h1 {
    font-family: 'Fredoka One', cursive;
    font-size: 64px;
    font-weight: normal;
    text-align: center;
  }

  h2 {
    font-family: 'Fredoka One', cursive;
    font-weight: normal;
    text-align: center;
  }


</style>
