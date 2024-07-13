pipeline {
    agent any
    stages {
        stage ('Build') {
            steps {
                sh 'echo "Building..."'
                sh 'chmod +x ./scripts/Linux-Build.sh'
                sh './scripts/Linux-Build.sh'
                archiveArtifacts artifacts: 'build/tests/XML_Lib_Unit_Tests', fingerprint: true
            }
        }
        stage ('Test'){
            steps {
                sh 'echo "Testing..."'
                sh 'chmod +x ./scripts/Linux-Run-Tests.sh'
                sh './scripts/Linux-Run-Tests.sh'
            }
        }
    }
}