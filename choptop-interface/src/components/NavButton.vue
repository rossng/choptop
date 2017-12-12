<template>
  <div :class="getClass()" v-on:click="doAction()"></div>
</template>

<script>
  export default {
    name: 'nav-button',
    props: ['direction', 'eventBus'],
    created(){
      this.eventBus.$on("pressed", this.handlePressed);
      this.eventBus.$on("pressStart", this.handlePressStart);
      this.eventBus.$on("pressCancel", this.handlePressCancel);
    },

    data() {
      return {
        pressStart: 0,
        pressed: false,
        pressState: "none"

      }
    },

    methods: {
      getClass: function () {
        return "nav-button " + "nav-button-" + this.direction + " pressed-" + this.pressState
      },
      doAction() {
        this.eventBus.$emit("pressed", this.direction)
      },
      handlePressed(dir){
        if(dir != this.direction) return;
        this.pressState = "success"
        console.log("pressed " + dir)
      },
      handlePressStart(dir){
        if(dir != this.direction) return;
        this.pressState = "start"
        console.log("press start " + dir)

      },
      handlePressCancel(dir){
        if(dir != this.direction) return;
        this.pressState = "cancel"
        console.log("press cancel")
      }
    },


  }
</script>


<style scoped>

  @keyframes buttonPress {
    0% {
      background-color: green;
    }
    70% {
      background-color: orange;
    }
    95% {
      background-color: red;
    }
    100% {
      background-color: green;
    }
  }

  .nav-button {
    color: white;
    background: none;
  }

  .nav-button-up {
    height: 20px;
  }

  .nav-button-up:before {
    color: #ffffff;
    content: '▲';
  }

  .nav-button-left {
    display: inline-block;
    height: 440px;
    width: 20px;
    float: left;
    position: relative;
  }

  .nav-button-left:before {
    color: #ffffff;
    content: '◄';
    position: absolute;
    top: 50%;
    left: 3px;
  }

  .nav-button-right {
    display: inline-block;
    height: 440px;
    width: 20px;
    float: left;
    position: relative;
  }

  .nav-button-right:before {
    color: #ffffff;
    content: '►';
    position: absolute;
    top: 50%;
    left: 3px;
  }

  .nav-button-down {
    clear: both;
    height: 20px;
  }

  .nav-button-down:before {
    color: #ffffff;
    content: '▼';
  }

  .pressed-start {
    /*animation-name: buttonPress;*/
    animation-duration: 2s;
    background-color: green;
  }

  .pressed-cancel{
    background:red;
  }

  .pressed-success {
    /*background:green;*/
    background:white;
  }
</style>
