<template>
  <div class="stepWrapper">
    <div class="middleRow">
      <div :class="{arr:true, leftarr:true, noshow: stepIdx === 0}">
        <icon name="angle-left" scale="4"></icon>
      </div>
      <div class="step">
        <p class="stepCount">Step {{stepIdx+1}} of {{steps.length}}</p>
        {{steps[stepIdx].text}}
      </div>
      <div :class="{rightArr:true, arr:true, noshow: stepIdx === steps.length - 1}">
        <icon name="angle-right" scale="4"></icon>
      </div>
    </div>
  </div>


</template>

<script>


  export default {
    name: 'stepDisplay',
    components: {},

    props: ['steps', 'stepIdx',],
    data() {
      return {
        selected: true,
        timeDisp: false,

      }
    },

    methods: {
      hasTime() {
        return this.steps[this.stepIdx].time !== undefined
      },
      hasWeight() {
        return this.steps[this.stepIdx].weighing === true;
      },

      hasInstructions() {
        return this.steps[this.stepIdx].extra !== undefined;
      },
    }
  }
</script>


<style scoped>
  .step {
    font-size: 36px;
    margin: 30px;
  }

  .stepCount {
    font-size: 24px;
    font-weight: medium;
  }

  .middleRow {
    display: flex;
    flex-direction: row;
    justify-content: space-between;
    padding: 10px;
    align-items: center;
  }

  .arr {
    font-size: 100px;
    opacity: 1;
    transition: 0.5s;
  }

  .arr.noshow {
    opacity: 0;
    transition: 0.5s;
  }

</style>
