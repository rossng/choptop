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
      handlePressNone(dir){
        if(dir != this.direction) return;
        this.pressState = "none"
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
  .nav-button {
    color: white;
    background: none;
  }

  .nav-button-up {
    height: 20px;
  }

  .nav-button-left {
    display: inline-block;
    height: 440px;
    width: 20px;
    float: left;
    position: relative;
  }

  .nav-button-right {
    display: inline-block;
    height: 440px;
    width: 20px;
    float: left;
    position: relative;
  }

  .nav-button-down {
    clear: both;
    height: 20px;
  }

  .pressed-start {
    animation-name: buttonPress;
    background-color: green;
    animation-duration: 5s;
  }

  /*.pressedCancelNone{
    animation-name: pressCancelNone;
    animation-duration: 1s;
  */

  .nav-button{
    background:lightgreen;
  }

  .pressed-cancel{
    transition: 0s;
    animation-name: pressCancel;
    animation-duration: 1s;
  }

  .pressed-success {
    animation-name: pressSuccess;
    animation-duration: 0.5s;
  }

  @keyframes buttonPress {
    0% {
      background-color: lightgreen;
    }
    100% {
      background-color: green;
    }
  }

  @keyframes pressCancel {
    0% {
      background-color: red;
    }

    40% {
      background-color: red;
    }

    100% {
      background-color: lightgreen;
    }
  }

  @keyframes pressSuccess {
    0% {
      background-color: green;
    }

    40% {
      background-color: green;
    }

    100% {
      background-color: lightgreen;
    }
  }


</style>
